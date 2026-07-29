// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroBase.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/LordHeroAttributeSet.h"
#include "Gameplay/Attributes/UnitBaseAttributes.h"
#include "Gameplay/Units/HeroEquipment.h"

AHeroBase::AHeroBase() : ACreature()
{
	Team = EUnitTeam::Player1;
	HeroXP = 0;

	LordHeroAttributeSet = CreateDefaultSubobject<ULordHeroAttributeSet>(TEXT("LordHeroAttributeSet"));
	Inventory = CreateDefaultSubobject<UHeroInventory>(TEXT("Hero Inventory"));
}

int AHeroBase::GetHeroMaxXP() const
{
	bool bIgnored;
	const int Level = FMath::Clamp((int) AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetLevelAttribute(), bIgnored), 1, 9999);
	return 100 + Level * 20;
}

void AHeroBase::BeginPlay()
{
	Super::BeginPlay();
	Inventory->InitInventory(EquipmentTypes);
	HandleInventoryChange();
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
