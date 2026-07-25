// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/LordHeroAttributeSet.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/Combat/Ability/LordUnitAttributeSet.h"

void ULordHeroAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULordHeroAttributeSet, MaxMana);
	DOREPLIFETIME(ULordHeroAttributeSet, Mana);

	DOREPLIFETIME(ULordHeroAttributeSet, StartingHealth);
	DOREPLIFETIME(ULordHeroAttributeSet, ExtraHealthPerLevel);

	DOREPLIFETIME(ULordHeroAttributeSet, Strength);
	DOREPLIFETIME(ULordHeroAttributeSet, Agility);
	DOREPLIFETIME(ULordHeroAttributeSet, Intelligence);
	DOREPLIFETIME(ULordHeroAttributeSet, Stamina);
}

void ULordHeroAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void ULordHeroAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp Mana to their maxes
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
	}

	// Clamp Characteristics to 1+ since some math formulas assume they are positive
	if (Attribute == GetStrengthAttribute()
		|| Attribute == GetAgilityAttribute()
		|| Attribute == GetIntelligenceAttribute()
		|| Attribute == GetStaminaAttribute()
		)
	{
		// Also inventing cap of 1000 :shrug:
		NewValue = FMath::Clamp(NewValue, 1, 1000);
	}
}

void ULordHeroAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// Changes that will recalculate max health
	if (Attribute == GetStaminaAttribute()
		|| Attribute == GetStartingHealthAttribute()
		|| Attribute == GetExtraHealthPerLevelAttribute()
		)
	{
		ResetBaseHealth();
	}

	// Changes that recalc mana
	if (Attribute == GetIntelligenceAttribute()
		|| Attribute == GetStartingHealthAttribute()
		|| Attribute == GetExtraHealthPerLevelAttribute()
		)
	{
		ResetBaseMana();
	}

	// If Max Mana changes, make sure to cap their active counterparts
	if (Attribute == GetMaxManaAttribute())
	{
		if (GetMana() < GetMaxMana())
		{
			SetMana(GetMaxMana());
		}
	}
}

void ULordHeroAttributeSet::Init(ULordUnitAttributeSet* InUnitAttribs, bool bInitUnitValues)
{
	this->UnitAttribs = InUnitAttribs;
	if (bInitUnitValues)
	{
		// Health and Mana are based on Stamina and Intelligence...
		// Proc the 'post attribute change' for these
		PostAttributeChange(GetStaminaAttribute(), 0, GetStamina());
		PostAttributeChange(GetIntelligenceAttribute(), 0, GetIntelligence());
	}
}

void ULordHeroAttributeSet::UpdateAfterLevelup()
{
	ResetBaseHealth();
	ResetBaseMana();
}

int ULordHeroAttributeSet::CalculateBaseHealth() const
{
	return GetStartingHealth() // Starting amount
		+ ((GetStamina() + GetExtraHealthPerLevel()) * (UnitAttribs->GetLevel() - 1)) // Amount-per-level (stamina + bonus) * number of level ups
		;
}

int ULordHeroAttributeSet::CalculateBaseMana() const
{
	// Note that mana uses same start + bonus amounts as health
	return GetStartingHealth() // Starting amount
		+ ((GetIntelligence() + GetExtraHealthPerLevel()) * (UnitAttribs->GetLevel() - 1)) // Amount-per-level (int + bonus) * number of level ups
		;
}

void ULordHeroAttributeSet::ResetBaseHealth()
{
	const int ExpectedValue = CalculateBaseHealth();
	UnitAttribs->SetMaxHealth(ExpectedValue);
}

void ULordHeroAttributeSet::ResetBaseMana()
{
	const int ExpectedValue = CalculateBaseMana();
	SetMaxMana(ExpectedValue);
}
