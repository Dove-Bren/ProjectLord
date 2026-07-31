// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroBase.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/LordHeroAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/HeroEquipment.h"

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
	return 10 + ((Level-1) * 5);
}

void AHeroBase::BeginPlay()
{
	Super::BeginPlay();
	Inventory->InitInventory(EquipmentTypes);
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
}

void AHeroBase::HandleInventoryChange()
{
	UnapplyInventoryAttributes();

	// Cache inventory
	LastAppliedInventoryDefs.Empty();

	LastAppliedInventoryDefs.Add(Inventory->GetWeaponAsStack()->GetItemDef());
	LastAppliedInventoryDefs.Add(Inventory->GetArmorAsStack()->GetItemDef());
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
	// TEST amount; should prob just be 1
	AddHeroXP(GetHeroMaxXP() / 2);
}
