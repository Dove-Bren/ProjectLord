// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroBase.h"

#include "GameplayEffect.h"
#include "Gameplay/GameGood.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/LordHeroAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/HeroEquipment.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"
#include "UI/ViewModels/Generic/LevelViewModel.h"
#include "UI/ViewModels/Generic/SummarySlotsViewModel.h"

AHeroBase::AHeroBase() : ACreature()
{
	Team = EGameTeam::Player1;
	HeroXP = 0;

	LordHeroAttributeSet = CreateDefaultSubobject<ULordHeroAttributeSet>(TEXT("LordHeroAttributeSet"));
	Inventory = CreateDefaultSubobject<UHeroInventory>(TEXT("Hero Inventory"));
}

int AHeroBase::GetHeroMaxXP() const
{
	bool bIgnored;
	const int Level = FMath::Clamp((int) AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetLevelAttribute(), bIgnored), 1, 9999);
	return 8 + ((Level-1) * 1);
}

void AHeroBase::BeginPlay()
{
	Super::BeginPlay();
	Inventory->InitInventory(StarterWeapon, StarterArmor);
	HandleInventoryChange();

	CombatComponent->OnAttackLand.AddDynamic(this, &AHeroBase::OnAttack);
}

void AHeroBase::SetupBaseAttributes()
{
	// Make sure to set up association early
	LordHeroAttributeSet->Init(CombatAttributeSet);

	// Let base class take care of setting up base values
	Super::SetupBaseAttributes();

	// Make sure to prompt attribute set to recalc dependent attributes
	LordHeroAttributeSet->UpdateDerivedUnitValues();

	// Set up mod for attributes
	{
		auto HeroAttributeSet = LordHeroAttributeSet;

		{
			UGameplayEffect* GE_StrMod = NewObject<UGameplayEffect>(this, TEXT("StrengthMod"));
			GE_StrMod->DurationPolicy = EGameplayEffectDurationType::Infinite;
			auto StrengthAttribute = HeroAttributeSet->GetStrengthAttribute();
			auto MeleeDmgAttribute = CombatAttributeSet->GetMeleeDamageAttribute();

			FGameplayModifierInfo Mod;
			Mod.Attribute = MeleeDmgAttribute;
			Mod.ModifierOp = EGameplayModOp::AddFinal;
			FAttributeBasedFloat Curve;
			Curve.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(StrengthAttribute, EGameplayEffectAttributeCaptureSource::Source, false);
			Curve.Coefficient = AHeroBase::DamageModPerAttribute;
			Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Curve);
			GE_StrMod->Modifiers.Add(MoveTemp(Mod));

			FGameplayEffectSpec Spec(GE_StrMod, AbilitySystemComponent->MakeEffectContext(), 1);
			StrengthDamageModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(Spec);
		}

		{
			UGameplayEffect* GE_AgiMod = NewObject<UGameplayEffect>(this, TEXT("AgilityMod"));
			GE_AgiMod->DurationPolicy = EGameplayEffectDurationType::Infinite;
			auto AgilityAttribute = HeroAttributeSet->GetAgilityAttribute();
			auto RangedDmgAttribute = CombatAttributeSet->GetRangedDamageAttribute();

			FGameplayModifierInfo Mod;
			Mod.Attribute = RangedDmgAttribute;
			Mod.ModifierOp = EGameplayModOp::AddFinal;
			FAttributeBasedFloat Curve;
			Curve.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(AgilityAttribute, EGameplayEffectAttributeCaptureSource::Source, false);
			Curve.Coefficient = AHeroBase::DamageModPerAttribute;
			Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Curve);
			GE_AgiMod->Modifiers.Add(MoveTemp(Mod));

			FGameplayEffectSpec Spec(GE_AgiMod, AbilitySystemComponent->MakeEffectContext(), 1);
			AgilityDamageModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(Spec);
		}

		{
			UGameplayEffect* GE_IntMod = NewObject<UGameplayEffect>(this, TEXT("IntelligenceMod"));
			GE_IntMod->DurationPolicy = EGameplayEffectDurationType::Infinite;
			auto IntelligenceAttribute = HeroAttributeSet->GetIntelligenceAttribute();
			auto MagicDmgAttribute = CombatAttributeSet->GetMagicDamageAttribute();

			FGameplayModifierInfo Mod;
			Mod.Attribute = MagicDmgAttribute;
			Mod.ModifierOp = EGameplayModOp::AddFinal;
			FAttributeBasedFloat Curve;
			Curve.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(IntelligenceAttribute, EGameplayEffectAttributeCaptureSource::Source, false);
			Curve.Coefficient = AHeroBase::DamageModPerAttribute;
			Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Curve);
			GE_IntMod->Modifiers.Add(MoveTemp(Mod));

			FGameplayEffectSpec Spec(GE_IntMod, AbilitySystemComponent->MakeEffectContext(), 1);
			IntelligenceDamageModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(Spec);
		}

	}
}

void AHeroBase::SetupSelectionData(USelectionComponent* InSelectionComponent)
{
	Super::SetupSelectionData(InSelectionComponent);

	auto GoldVM = CreateLordVM<UVMGold>(this);
	Inventory->OnInventoryGoldChanged.AddWeakLambda(this, [this, GoldVM]() {
			GoldVM->SetGold(Inventory->GetPersonalGold());
		});
	GoldVM->SetGold(Inventory->GetPersonalGold());
	InSelectionComponent->SetGoldVM(GoldVM);

	bool bIgnored;
	auto LevelAttribute = GetCombatAttributeSet()->GetLevelAttribute();
	auto LevelVM = CreateLordVM<UVMLevel>(this);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(LevelAttribute)
		.AddWeakLambda(this, [LevelVM](const FOnAttributeChangeData& ChangeData)
			{
				LevelVM->SetLevel(static_cast<int>(ChangeData.NewValue));
			});
	LevelVM->SetLevel(AbilitySystemComponent->GetGameplayAttributeValue(LevelAttribute, bIgnored));
	OnXPChange.AddWeakLambda(this, [this, LevelVM]() {
			LevelVM->SetProgress(GetHeroXPPercent());
		});
	LevelVM->SetProgress(GetHeroXPPercent());
	InSelectionComponent->SetLevelVM(LevelVM);

	auto SlotsVM = CreateLordVM<UVMSummarySlots>(this);
	SlotsVM->Init();
	Inventory->OnInventoryItemsChanged.AddWeakLambda(this, [this, SlotsVM]() {
			SlotsVM->SetSlot(0, UVMSummarySlot::MakeItem(this, Inventory->GetWeapon()));
			SlotsVM->SetSlot(1, UVMSummarySlot::MakeItem(this, Inventory->GetHealthPotions()));
			SlotsVM->SetSlot(2, Inventory->GetExtraSlots().IsValidIndex(0) ? UVMSummarySlot::MakeItem(this, Inventory->GetExtraSlots()[0]) : UVMSummarySlot::MakeEmpty(this));
			SlotsVM->SetSlot(3, UVMSummarySlot::MakeItem(this, Inventory->GetArmor()));
			SlotsVM->SetSlot(4, UVMSummarySlot::MakeItem(this, Inventory->GetManaPotions()));
			SlotsVM->SetSlot(2, Inventory->GetExtraSlots().IsValidIndex(1) ? UVMSummarySlot::MakeItem(this, Inventory->GetExtraSlots()[1]) : UVMSummarySlot::MakeEmpty(this));
		});

	// Ugly optimization; inventory isn't actually set up yet, so don't bother doing this yet
	/*SlotsVM->SetSlot(0, UVMSummarySlot::MakeItem(this, Inventory->GetWeapon()));
	SlotsVM->SetSlot(1, UVMSummarySlot::MakeItem(this, Inventory->GetHealthPotions()));
	SlotsVM->SetSlot(2, Inventory->GetExtraSlots().IsValidIndex(0) ? UVMSummarySlot::MakeItem(this, Inventory->GetExtraSlots()[0]) : UVMSummarySlot::MakeEmpty(this));
	SlotsVM->SetSlot(3, UVMSummarySlot::MakeItem(this, Inventory->GetArmor()));
	SlotsVM->SetSlot(4, UVMSummarySlot::MakeItem(this, Inventory->GetManaPotions()));
	SlotsVM->SetSlot(2, Inventory->GetExtraSlots().IsValidIndex(1) ? UVMSummarySlot::MakeItem(this, Inventory->GetExtraSlots()[1]) : UVMSummarySlot::MakeEmpty(this));*/

	InSelectionComponent->SetSlotsVM(SlotsVM);
}

void AHeroBase::HandleInventoryChange()
{
	UnapplyInventoryAttributes();

	// Cache inventory
	LastAppliedInventoryDefs.Empty();

	LastAppliedInventoryDefs.Add(Inventory->GetWeapon()->GetItemDef());
	LastAppliedInventoryDefs.Add(Inventory->GetArmor()->GetItemDef());
	for (const auto ExtraStack : Inventory->GetExtraSlots())
	{
		LastAppliedInventoryDefs.Add(ExtraStack->GetItemDef());
	}

	// Apply attributes
	ApplyInventoryAttributes();
}

void AHeroBase::UnapplyInventoryAttributes()
{
	for (const auto Def : LastAppliedInventoryDefs)
	{
		for (auto& AttributeMod : Def->GetAttributesToApply())
		{
			if (!AbilitySystemComponent->HasAttributeSetForAttribute(AttributeMod.Attribute))
			{
				continue; // Not an error;
			}

			// Don't support variation from items
			ensure(AttributeMod.Variation == 0);

			AbilitySystemComponent->ApplyModToAttribute(AttributeMod.Attribute, EGameplayModOp::AddFinal, -AttributeMod.BaseValue);
		}
	}
}

void AHeroBase::ApplyInventoryAttributes()
{
	for (const auto Def : LastAppliedInventoryDefs)
	{
		for (auto& AttributeMod : Def->GetAttributesToApply())
		{
			if (!AbilitySystemComponent->HasAttributeSetForAttribute(AttributeMod.Attribute))
			{
				continue; // Not an error;
			}

			// Don't support variation from items
			ensure(AttributeMod.Variation == 0);

			AbilitySystemComponent->ApplyModToAttribute(AttributeMod.Attribute, EGameplayModOp::AddFinal, AttributeMod.BaseValue);
		}
	}
}

void AHeroBase::AddHeroXP(int Amount)
{
	if (!ensure(Amount > 0))
	{
		return;
	}

	HeroXP += Amount;
	if (HeroXP >= GetHeroMaxXP())
	{
		DoLevelUp();
	}
	OnXPChange.Broadcast();
}

void AHeroBase::DoLevelUp()
{
	HeroXP = 0;

	bool bIgnored;
	const int Level = AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetLevelAttribute(), bIgnored);
	AbilitySystemComponent->SetNumericAttributeBase(CombatAttributeSet->GetLevelAttribute(), Level + 1);

	// Update derived attributes
	LordHeroAttributeSet->UpdateDerivedUnitValues();
	OnLevelUp();
}

void AHeroBase::OnAttack(AActor* TargetActor, UCombatComponent* TargetCombatComponent)
{
	constexpr int BaseGain = 1;
	AddHeroXP(BaseGain);
}

bool AHeroBase::CanApply(const UGameGood* Good) const
{
	// If good has an item, return true;
	// This should be where we look at item caps maybe, but leaving
	// it off for now.
	// Either this will have to return false (too many health potions already!)
	// or the AI will have to decide it doesn't want anymore if it already
	// has enough
	if (IsValid(Good->GetItemDef()))
	{
		return true;
	}

	// Else check if we already have the ability
	auto Ability = Good->GetAbility();
	if (IsValid(Ability))
	{
		return false;
	}

	auto AbilityHandle = AbilitySystemComponent->FindAbilitySpecFromClass(Ability);
	if (AbilityHandle)
	{
		return false;
	}

	return true;
}

void AHeroBase::Apply(const UGameGood* Good)
{
	if (IsValid(Good->GetItemDef()))
	{
		auto Stack = UHeroItemStack::Make(this, Good->GetItemDef(), 1);
		GetInventory()->Add(Stack);
	}

	auto Ability = Good->GetAbility();
	if (IsValid(Ability))
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
	}
}

void AHeroBase::UpdateAttributeDamageMod(FActiveGameplayEffectHandle& AttributeHandle, int Level)
{
	AbilitySystemComponent->SetActiveGameplayEffectLevel(AttributeHandle, Level);
}

float AHeroBase::DamageModPerAttribute = (1.0f / 3.0f);
