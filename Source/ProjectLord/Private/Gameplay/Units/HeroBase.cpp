// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroBase.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/GameGood.h"
#include "Gameplay/GameTeamState.h"
#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Interaction/SelectionComponent.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Attributes/AttributeListener.h"
#include "Gameplay/Attributes/CreatureAttributeSet.h"
#include "Gameplay/Attributes/LordHeroAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Combat/GameplayEffect/AttributeBasedDamageGameplayEffect.h"
#include "Gameplay/Combat/GameplayEffect/HeroLevelGameplayEffect.h"
#include "Gameplay/Combat/GameplayEffect/LevelBasedConfidenceGameplayEffect.h"
#include "Gameplay/Units/HeroEquipment.h"
#include "Gameplay/Units/HeroInventory.h"
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

void AHeroBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Inventory);
	DOREPLIFETIME(ThisClass, HeroXP);
	DOREPLIFETIME(ThisClass, HeroName);
	DOREPLIFETIME(ThisClass, CurrentFlagTarget);
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

void AHeroBase::Sided_RegisterAttributes()
{
	Super::Sided_RegisterAttributes();

	// Make sure to set up association early
	LordHeroAttributeSet->Init(CombatAttributeSet);
}

void AHeroBase::Server_SetupBaseAttributes()
{
	// Let base class take care of setting up base values
	Super::Server_SetupBaseAttributes();

	// Make sure to prompt attribute set to recalc dependent attributes
	LordHeroAttributeSet->UpdateDerivedUnitValues();

	// Set up mod for attributes
	{
		auto SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(UGEStrengthBasedMeleeDamage::StaticClass(), 1, AbilitySystemComponent->MakeEffectContext());
		if (ensure(SpecHandle.IsValid()))
		{
			StrengthDamageModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}

		SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(UGEAgilityBasedRangedDamage::StaticClass(), 1, AbilitySystemComponent->MakeEffectContext());
		if (ensure(SpecHandle.IsValid()))
		{
			AgilityDamageModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}

		SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(UGEIntelligenceBasedMagicDamage::StaticClass(), 1, AbilitySystemComponent->MakeEffectContext());
		if (ensure(SpecHandle.IsValid()))
		{
			IntelligenceDamageModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}

	// Set up confidence
	auto SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(UGELevelBasedConfidence::StaticClass(), 1, AbilitySystemComponent->MakeEffectContext());
	if (SpecHandle.IsValid())
	{
		ConfidenceLevelModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}

	// And level-up stat modifier
	if (ensure(HeroLevelGameplayEffect))
	{
		SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(HeroLevelGameplayEffect, 1, AbilitySystemComponent->MakeEffectContext());
		if (SpecHandle.IsValid())
		{
			LevelModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}

void AHeroBase::Client_SetupSelectionData(USelectionComponent* InSelectionComponent)
{
	Super::Client_SetupSelectionData(InSelectionComponent);

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
	OnHeroXPChange.AddWeakLambda(this, [this, LevelVM]() {
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
			SlotsVM->SetSlot(5, Inventory->GetExtraSlots().IsValidIndex(1) ? UVMSummarySlot::MakeItem(this, Inventory->GetExtraSlots()[1]) : UVMSummarySlot::MakeEmpty(this));
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

UVMUnit* AHeroBase::Client_ConstructUnitVM()
{
	return UVMHero::Make(this);
}

void AHeroBase::Client_InitUnitVM()
{
	Super::Client_InitUnitVM();

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
			LordController->AddToastNotification(FToastNotification(EToastNotificationType::HeroDead, HeroType->UnitIcon, GetHeroName(), SelectionComponent));
		}
	}
}

void AHeroBase::OnFinalDeath()
{
	if (HasAuthority())
	{
		if (auto TeamState = GetTeamState())
		{
			TeamState->AddDeadHero(/*this*/);
		}
	}

	Super::OnFinalDeath();
}

void AHeroBase::HandleInventoryChange()
{
	// TODO: Check if anything actually changed and only
	// change based on the changes? Like map effect handles to the item,
	// get a list of changes, and then remove/apply as needed.

	if (HasAuthority())
	{

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
}

void AHeroBase::UnapplyInventoryAttributes()
{
	if (!ensure(HasAuthority())) return;
	for (const auto Handle : LastAppliedInventoryEffects)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
	}
	LastAppliedInventoryEffects.Empty();
}

void AHeroBase::ApplyInventoryAttributes()
{
	if (!ensure(HasAuthority())) return;
	for (const auto Def : LastAppliedInventoryDefs)
	{
		auto Context = AbilitySystemComponent->MakeEffectContext();
		for (auto& Effect : Def->GetItemEffects())
		{
			auto SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, Context);
			if (SpecHandle.IsValid())
			{
				LastAppliedInventoryEffects.Add(AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data));
			}
		}
	}
}

void AHeroBase::AddHeroXP(int Amount)
{
	if (!ensure(HasAuthority())) return;
	if (!ensure(Amount > 0))
	{
		return;
	}

	HeroXP += Amount;
	if (HeroXP >= GetHeroMaxXP())
	{
		DoLevelUp();
	}
	OnRep_HeroXP();
}

void AHeroBase::DoLevelUp()
{
	if (!ensure(HasAuthority())) return;
	HeroXP = 0;

	const int Level = GetHeroLevel();
	AbilitySystemComponent->SetNumericAttributeBase(CombatAttributeSet->GetLevelAttribute(), Level + 1);

	// Update derived attributes
	LordHeroAttributeSet->UpdateDerivedUnitValues();
	UpdateConfidenceLevelMod();
	OnLevelUp();
}

void AHeroBase::OnLevelUp_Implementation()
{
	OnHeroLevelUp.Broadcast(GetHeroLevel());
	BP_OnLevelUp();
}

void AHeroBase::OnGoldAwarded_Implementation(int InAmount)
{
	BP_OnGoldAwarded(InAmount);
}

void AHeroBase::OnHealthPotionUsed_Implementation()
{
	BP_OnHealthPotionUsed();
}

void AHeroBase::OnManaPotionUsed_Implementation()
{
	BP_OnManaPotionUsed();
}

void AHeroBase::OnAttack(AActor* TargetActor, UCombatComponent* TargetCombatComponent)
{
	if (HasAuthority())
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
}

void AHeroBase::OnHealthChanged(int Health, int MaxHealth)
{
	if (HasAuthority())
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
}

bool AHeroBase::AttemptUseHealthPotion()
{
	if (!ensure(HasAuthority())) return false;

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
	if (!ensure(HasAuthority())) return false;
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

void AHeroBase::Server_HandleUnitRecruited()
{
	Super::Server_HandleUnitRecruited();

	HeroName = GenerateHeroName();
	OnRep_HeroName();
}

void AHeroBase::OnRep_HeroName()
{
	if (GetUnitVM())
	{
		Cast<UVMHero>(GetUnitVM())->SetHeroName(HeroName);
	}
		SelectionComponent->SetCustomName(GetHeroName());
}

void AHeroBase::OnRep_HeroXP()
{
	OnHeroXPChange.Broadcast();
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
	// Note: goods have to either have an item or ability with this check
	auto Ability = Good->GetAbility();
	if (!IsValid(Ability))
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
	if (!ensure(HasAuthority())) return;
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
	if (!ensure(HasAuthority())) return;
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

void AHeroBase::UpdateConfidenceLevelMod()
{
	if (!ensure(HasAuthority())) return;
	if (!ConfidenceLevelModHandle.IsValid()) return;

	// TODO: delete after verifying this works the same.
	// Used to have to be updated manually every level, but now I set the mod to be the level.
	//AbilitySystemComponent->SetActiveGameplayEffectLevel(ConfidenceLevelModHandle, GetHeroLevel());
}

bool AHeroBase::PurchaseGood(FGoodOffer Offer)
{
	if (!ensure(HasAuthority())) return false;
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
			CombatComponent->GiveCombatAbility(Ability);
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
	if (!ensure(HasAuthority())) return;
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

