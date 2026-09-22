// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroBase.h"

#include "GameplayEffect.h"
#include "Gameplay/GameGood.h"
#include "Gameplay/LordPlayerController.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Attributes/AttributeListener.h"
#include "Gameplay/Attributes/CreatureAttributeSet.h"
#include "Gameplay/Attributes/LordHeroAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/HeroEquipment.h"
#include "UI/InspectWidget.h"
#include "UI/ToastNotification.h"
#include "UI/WidgetBlueprintClassRegistry.h"
#include "UI/ViewModels/Units/HeroViewModel.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"
#include "UI/ViewModels/Generic/InspectableViewModel.h"
#include "UI/ViewModels/Generic/LevelViewModel.h"
#include "UI/ViewModels/Generic/SummarySlotsViewModel.h"

AHeroBase::AHeroBase() : ACreature()
{
	Team = EGameTeam::Player1;
	HeroXP = 0;
	HeroName = FText::FromString(TEXT("Hero"));

	LordHeroAttributeSet = CreateDefaultSubobject<ULordHeroAttributeSet>(TEXT("LordHeroAttributeSet"));
	Inventory = CreateDefaultSubobject<UHeroInventory>(TEXT("Hero Inventory"));
	Action = ECreatureAction::HeroIdle;
	bEvacuatesOnAttack = true;
}

int AHeroBase::GetHeroMaxXP() const
{
	const int Level = FMath::Clamp(GetHeroLevel(), 1, 9999);
	return 8 + ((Level-1) * 1);
}

void AHeroBase::BeginPlay()
{
	Super::BeginPlay();
	Inventory->InitInventory(StarterWeapon, StarterArmor);
	Cast<UVMHero>(GetUnitVM())->SetInventory(GetInventory()->GetViewModel());
	Inventory->OnInventoryItemsChanged.AddUObject(this, &ThisClass::HandleInventoryChange);
	HandleInventoryChange();

	CombatComponent->OnAttackLand.AddDynamic(this, &AHeroBase::OnAttack);
	CombatComponent->OnHealthChange.AddDynamic(this, &ThisClass::OnHealthChanged);
}

void AHeroBase::NotifyResidenceAttacked(AResidentialBuilding* Building, AActor* AttackingActor, UCombatComponent* AttackingCombatComponent)
{
	Super::NotifyResidenceAttacked(Building, AttackingActor, AttackingCombatComponent);
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

	// Set up confidence
	{
		auto HeroAttributeSet = LordHeroAttributeSet;

		UGameplayEffect* GE_ConfidenceMod = NewObject<UGameplayEffect>(this, TEXT("ConfidenceLevelMod"));
		GE_ConfidenceMod->DurationPolicy = EGameplayEffectDurationType::Infinite;
		auto ConfidenceAttribute = HeroAttributeSet->GetConfidenceAttribute();

		FGameplayModifierInfo Mod;
		Mod.Attribute = ConfidenceAttribute;
		Mod.ModifierOp = EGameplayModOp::AddBase;
		Mod.ModifierMagnitude = FScalableFloat(1);
		GE_ConfidenceMod->Modifiers.Add(MoveTemp(Mod));

		FGameplayEffectSpec Spec(GE_ConfidenceMod, AbilitySystemComponent->MakeEffectContext(), GetHeroLevel());
		ConfidenceLevelModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(Spec);
	}
}

void AHeroBase::SetupSelectionData(USelectionComponent* InSelectionComponent)
{
	Super::SetupSelectionData(InSelectionComponent);

	InSelectionComponent->SetCustomName(GetHeroName());

	auto GoldVM = CreateLordVM<UVMGold>(this);
	Inventory->OnInventoryGoldChanged.AddWeakLambda(this, [this, GoldVM]() {
			GoldVM->SetGold(Inventory->GetPersonalGold());
			GoldVM->SetTaxGold(Inventory->GetGuildGold());
		});
	GoldVM->SetGold(Inventory->GetPersonalGold());
	GoldVM->SetTaxGold(Inventory->GetGuildGold());
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

	const UWidgetBlueprintClassRegistry* WidgetBlueprints = UWidgetBlueprintClassRegistry::Get();
	const TSubclassOf<UInspectWidget> HeroInspectWidgetClass = WidgetBlueprints ? WidgetBlueprints->HeroInspectWidget.LoadSynchronous() : nullptr;
	if (ensure(HeroInspectWidgetClass))
	{
		auto InspectVM = UVMInspectable::Make(this, HeroInspectWidgetClass, FGetInspectVM::CreateWeakLambda(this, [this]() {
			return GetUnitVM();
		}));
		InSelectionComponent->SetInspectVM(InspectVM);
	}

	InSelectionComponent->SetSlotsVM(SlotsVM);
}

UVMUnit* AHeroBase::ConstructUnitVM()
{
	return UVMHero::Make(this);
}

void AHeroBase::InitUnitVM()
{
	Super::InitUnitVM();

	auto HeroVM = Cast<UVMHero>(GetUnitVM());
	HeroVM->SetHeroName(GetHeroName());
	HeroVM->SetLevelVM(SelectionComponent->GetLevelVM());
	HeroVM->SetInventory(GetInventory()->GetViewModel());

	// TODO: Abilities
	// CombatAbility->OnAbilitiesChanged().Add(...);

	// Attributes
	{
		auto ASC = AbilitySystemComponent;
		auto AttributeSet = LordHeroAttributeSet;
		REGISTER_ATTR_LISTENER(Strength, HeroVM, FAttributeListener::FSetIntValue::CreateWeakLambda(HeroVM, [HeroVM](int NewValue) {
			HeroVM->SetStrength(NewValue);
		}));
		REGISTER_ATTR_LISTENER(Agility, HeroVM, FAttributeListener::FSetIntValue::CreateWeakLambda(HeroVM, [HeroVM](int NewValue) {
			HeroVM->SetAgility(NewValue);
		}));
		REGISTER_ATTR_LISTENER(Intelligence, HeroVM, FAttributeListener::FSetIntValue::CreateWeakLambda(HeroVM, [HeroVM](int NewValue) {
			HeroVM->SetIntelligence(NewValue);
		}));
		REGISTER_ATTR_LISTENER(Stamina, HeroVM, FAttributeListener::FSetIntValue::CreateWeakLambda(HeroVM, [HeroVM](int NewValue) {
			HeroVM->SetStamina(NewValue);
		}));
	}
	{
		auto ASC = AbilitySystemComponent;
		auto AttributeSet = CreatureAttributeSet;
		REGISTER_ATTR_LISTENER(Movement, HeroVM, FAttributeListener::FSetFloatValue::CreateWeakLambda(HeroVM, [HeroVM](int NewValue) {
			HeroVM->SetMovement(NewValue);
			}));
	}
}

int AHeroBase::GetHeroLevel() const
{
	bool bIgnored;
	return FMath::FloorToInt(AbilitySystemComponent->GetGameplayAttributeValue(UCombatAttributeSet::GetLevelAttribute(), bIgnored));
}

void AHeroBase::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	if (auto TeamState = GetTeamState())
	{
		if (auto LordController = TeamState->GetPrimaryPlayerController())
		{
			auto HeroType = GetUnitType();
			LordController->AddToastNotification(FToastNotification(EToastNotificationType::HeroDead, HeroType->UnitIcon, GetHeroName()));
		}
	}
}

void AHeroBase::OnFinalDeath()
{
	if (auto TeamState = GetTeamState())
	{
		TeamState->AddDeadHero(/*this*/);
	}

	Super::OnFinalDeath();
}

void AHeroBase::HandleInventoryChange()
{
	// TODO: Check if anything actually changed and only
	// change based on the changes? Like map effect handles to the item,
	// get a list of changes, and then remove/apply as needed.

	UnapplyInventoryAttributes();

	// Cache inventory
	LastAppliedInventoryDefs.Empty();
	check(LastAppliedInventoryEffects.IsEmpty());

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
	for (const auto Handle : LastAppliedInventoryEffects)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
	}
	LastAppliedInventoryEffects.Empty();
}

void AHeroBase::ApplyInventoryAttributes()
{
	for (const auto Def : LastAppliedInventoryDefs)
	{
		auto Context = AbilitySystemComponent->MakeEffectContext();
		for (auto& Effect : Def->GetItemEffects())
		{
			auto EffectInstance = NewObject<UGameplayEffect>(this, Effect);
			LastAppliedInventoryEffects.Add(AbilitySystemComponent->ApplyGameplayEffectToSelf(EffectInstance, 1, Context));
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

	const int Level = GetHeroLevel();
	AbilitySystemComponent->SetNumericAttributeBase(CombatAttributeSet->GetLevelAttribute(), Level + 1);

	// Update derived attributes
	LordHeroAttributeSet->UpdateDerivedUnitValues();
	UpdateConfidenceLevelMod();
	OnLevelUp.Broadcast(Level);
	BP_OnLevelUp();
}

void AHeroBase::OnAttack(AActor* TargetActor, UCombatComponent* TargetCombatComponent)
{
	constexpr int BaseGain = 1;
	AddHeroXP(BaseGain);

	if (IsAlive() && ShouldUseManaPotion())
	{
		if (AttemptUseManaPotion())
		{
			OnManaPotionUsed();
		}
	}
}

void AHeroBase::OnHealthChanged(int Health, int MaxHealth)
{
	// Check if we need to use a potion
	if (IsAlive() && ShouldUseHealthPotion())
	{
		if (AttemptUseHealthPotion())
		{
			OnHealthPotionUsed();
		}
	}
}

bool AHeroBase::AttemptUseHealthPotion()
{
	auto Potions = Inventory->GetHealthPotions();
	if (!Potions || Potions->IsEmpty())
	{
		return false;
	}

	auto Def = Potions->GetItemDef();
	auto UseAbility = Def->GetUseAbility();
	if (!ensure(UseAbility))
	{
		return false;
	}

	FGameplayAbilitySpec AbilitySpec(UseAbility);
	AbilitySystemComponent->GiveAbilityAndActivateOnce(AbilitySpec);
	Potions->DecrementCount();
	return true;
}

bool AHeroBase::AttemptUseManaPotion()
{
	auto Potions = Inventory->GetManaPotions();
	if (!Potions || Potions->IsEmpty())
	{
		return false;
	}

	auto Def = Potions->GetItemDef();
	auto UseAbility = Def->GetUseAbility();
	if (!ensure(UseAbility))
	{
		return false;
	}

	FGameplayAbilitySpec AbilitySpec(UseAbility);
	AbilitySystemComponent->GiveAbilityAndActivateOnce(AbilitySpec);
	Potions->DecrementCount();
	return true;
}

bool AHeroBase::HeroShouldFleeFromNearby() const
{
	const int Threat = GetCombinedNearbyEnemyThreat();
	if (Threat <= 0)
	{
		return false;
	}

	const int Confidence = GetHeroCombinedConfidence();
	return Confidence < Threat;
}

int AHeroBase::GetCombinedNearbyEnemyThreat() const
{
	float AdjustedLevelSum = 0;
	for (auto Enemy : CombatComponent->GetNearbyEnemies())
	{
		// TODO: Add threat!
		AdjustedLevelSum += Enemy->GetThreat();
	}
	return AdjustedLevelSum;
}

int AHeroBase::GetHeroSoloConfidence() const
{
	bool bIgnored;
	return FMath::FloorToInt(AbilitySystemComponent->GetGameplayAttributeValue(ULordHeroAttributeSet::GetConfidenceAttribute(), bIgnored));
}

int AHeroBase::GetHeroCombinedConfidence() const
{
	const int HeroConfidence = GetHeroSoloConfidence();

	int NearbyAllyLevel = 0;
	for (auto Ally : CombatComponent->GetNearbyAllies())
	{
		if (auto AllyHero = Cast<AHeroBase>(Ally->GetOwner()))
		{
			NearbyAllyLevel += AllyHero->GetHeroLevel();
		}
		else
		{
			NearbyAllyLevel += 1;
		}
	}

	return HeroConfidence + NearbyAllyLevel;
}

bool AHeroBase::ShouldFlee() const
{
	if (Super::ShouldFlee())
	{
		return true;
	}

	if (HasBuilding() && HeroShouldFleeFromNearby())
	{
		return true;
	}

	return false;
}

void AHeroBase::HandleUnitRecruited()
{
	Super::HandleUnitRecruited();

	HeroName = GenerateHeroName();
	Cast<UVMHero>(GetUnitVM())->SetHeroName(HeroName);
	SelectionComponent->SetCustomName(GetHeroName());
}

FText AHeroBase::GenerateHeroName_Implementation() const
{
	const int FirstCount = FirstNames.Num();
	const int LastCount = LastNames.Num();

	if (FirstCount <= 0 || LastCount <= 0)
	{
		return FText::FromString(TEXT("Generic Hero"));
	}

	const FString& FirstName = FirstNames[FMath::RandRange(0, FirstCount - 1)];
	const FString& LastName = LastNames[FMath::RandRange(0, LastCount - 1)];
	return FText::FromString(FString::Printf(TEXT("%s %s"), *FirstName, *LastName));
}

bool AHeroBase::CanApply(const UGameGood* Good) const
{
	// Respect a unit type filter, if present
	if (const auto* TypeRestriction = Good->GetUnitTypeRestriction())
	{
		if (TypeRestriction != GetUnitType())
		{
			return false;
		}
	}

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

	if (HasAbility(Ability))
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

void AHeroBase::AwardGold(int Amount)
{
	const int Guild = Amount / 2;
	const int Personal = Amount - Guild;
	Inventory->AddPersonalGold(Personal);
	Inventory->AddGuildGold(Guild);

	OnGoldAwarded(Amount);
}

bool AHeroBase::ShouldUseHealthPotion_Implementation() const
{
	bool bIgnored;
	const float Health = AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetHealthAttribute(), bIgnored);
	const float MaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetMaxHealthAttribute(), bIgnored);

	return Health < (MaxHealth * .2f);
}

bool AHeroBase::ShouldUseManaPotion_Implementation() const
{
	bool bIgnored;
	const float Mana = AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetManaAttribute(), bIgnored);
	const float MaxMana = AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetMaxManaAttribute(), bIgnored);

	return Mana < (MaxMana * .2f);
}

EHeroDesireLevel AHeroBase::CheckDesireLevelForItem(const UHeroItemDef* Item) const
{
	if (!CanAcceptItem(Item))
	{
		return EHeroDesireLevel::NotApplicable;
	}

	const auto ItemType = Item->GetItemType();
	if (EItemType::Armor == ItemType || EItemType::Weapon == ItemType)
	{
		if (HasItemOrBetter(Item))
		{
			return EHeroDesireLevel::NotApplicable;
		}
	}

	// This should be personal preference time.

	// For now, I'll say heroes will have a strong desire for equipment or potions (if they have none)
	// They will have some desire for other goods and equipment they can use.
	// Otherwise, they will not desire anything else.
	if (EItemType::Armor == ItemType || EItemType::Weapon == ItemType)
	{
		return EHeroDesireLevel::StrongLike;
	}
	if ((EItemType::HealthPotion == ItemType && Inventory->GetNumHealthPotions() <= 0)
		|| (EItemType::ManaPotion == ItemType && Inventory->GetNumManaPotions() <= 0))
	{
		return EHeroDesireLevel::StrongLike;
	}

	if (EItemType::Other == ItemType)
	{
		return EHeroDesireLevel::Like;
	}

	// Basically just extra potions here
	return EHeroDesireLevel::Neutral;
}

bool AHeroBase::HasItemOrBetter(const UHeroItemDef* Item) const
{
	auto ItemType = Item->GetItemType();
	switch (ItemType)
	{
	case EItemType::Weapon:
	case EItemType::Armor:
		if (auto Equip = Cast<UHeroEquipmentDef>(Item))
		{
			if (Equip->GetEquipmentArchtype() != GetHeroEquipmentType())
			{
				// We couldn't equip, so whatever we have is better
				return true;
			}
			auto MyTier = (ItemType == EItemType::Weapon) ? Inventory->GetWeaponTier() : Inventory->GetArmorTier();
			return Equip->GetEquipmentTier() <= MyTier;
		}
		return false;
	case EItemType::HealthPotion:
		return Inventory->GetNumHealthPotions() > 0;
	case EItemType::ManaPotion:
		return Inventory->GetNumManaPotions() > 0;
	case EItemType::Other:
	default:
		break; // fall through
	}
	return Inventory->HasItem(Item);
}

bool AHeroBase::CanAcceptItem(const UHeroItemDef* Item) const
{
	if (auto Equip = Cast<UHeroEquipmentDef>(Item))
	{
		return Equip->GetEquipmentArchtype() == GetHeroEquipmentType();
	}

	const auto ItemType = Item->GetItemType();
	if (EItemType::HealthPotion == ItemType)
	{
		return Inventory->GetNumHealthPotions() < 9;
	}
	if (EItemType::ManaPotion == ItemType)
	{
		return CombatComponent->GetAbilitiesRequireMana()
			&& Inventory->GetNumManaPotions() < 9;
	}
	auto ItemStack = UHeroItemStack::Make(Inventory, Item, 1);
	return Inventory->CanFit(ItemStack);
}

EHeroDesireLevel AHeroBase::CheckDesireLevelForGood(const UGameGood* Good) const
{
	if (!CanApply(Good))
	{
		return EHeroDesireLevel::NotApplicable;
	}

	if (auto Item = Good->GetItemDef())
	{
		return CheckDesireLevelForItem(Item);
	}

	return EHeroDesireLevel::Like;
}

bool AHeroBase::HasAbility(TSubclassOf<UGameplayAbility> AbilityClass) const
{
	return !!AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);
}

void AHeroBase::UpdateAttributeDamageMod(FActiveGameplayEffectHandle& AttributeHandle, int Level)
{
	AbilitySystemComponent->SetActiveGameplayEffectLevel(AttributeHandle, Level);
}

void AHeroBase::UpdateConfidenceLevelMod()
{
	AbilitySystemComponent->SetActiveGameplayEffectLevel(ConfidenceLevelModHandle, GetHeroLevel());
}

bool AHeroBase::PurchaseGood(FGoodOffer Offer)
{
	if (ensure(Offer.Good))
	{
		Inventory->AddPersonalGold(-Offer.GoldCost);
		if (auto Item = Offer.Good->GetItemDef())
		{
			UHeroItemStack* Stack = UHeroItemStack::Make(Inventory, Item, 1);
			Inventory->Add(Stack);
		}

		if (auto Ability = Offer.Good->GetAbility())
		{
			FGameplayAbilitySpec Spec(Ability);
			AbilitySystemComponent->GiveAbility(Spec);
		}

		return true;
	}

	return false;
}

int AHeroBase::ScoreFlag(ARewardFlag* Flag) const
{
	int Score = 0;
	if (ensure(Flag) && Flag->GetReward() > 0)
	{
		const double Dist = FVector::DistXY(Flag->GetActorLocation(), GetActorLocation());

		// Heroes, in general, are more eager to go for flags that are close.
		// As they level up, they are less eager to go for flags in general and require
		// more money.
		// Some heroes have affinities for some times of flags as well.
		//
		// I'm thinking every 100 gold is worth +1 point for a flag. A hero's bonus might
		// also increase the score. And then after that, distance, hero level, etc. are negatives.

		Score = Flag->GetReward() / 100;
		if (const int* HeroBonus = RewardFlagBonus.Find(Flag->GetFlagType()))
		{
			Score += *HeroBonus;
		}
		
		int Level = GetHeroLevel();
		const int LevelPenalty = FMath::Max(0, Level - 5); // Start reducing 1x per level after lvl 5
		Score -= LevelPenalty;

		constexpr double UU_PER_SCORE = 2500.0; // This is UU and is very much a magic number
		int DistPenalty = FMath::FloorToInt(Dist / UU_PER_SCORE);
		Score -= DistPenalty;
	}

	return Score;
}

void AHeroBase::SetFlagTarget(ARewardFlag* Flag)
{
	if (CurrentFlagTarget.IsValid())
	{
		if (auto CurFlag = CurrentFlagTarget.Pin())
		{
			CurFlag->RemoveInterestedUnit(this);
		}
	}

	CurrentFlagTarget = Flag;
	if (Flag)
	{
		CurrentFlagTarget->AddInterestedUnit(this);
	}
}

float AHeroBase::DamageModPerAttribute = (1.0f / 3.0f);
