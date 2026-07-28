// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/LordHeroAttributeSet.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/Units/LordUnitAttributeSet.h"

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

	ROUND_ATTRIB_TO_INT(MaxMana);
	ROUND_ATTRIB_TO_INT(Mana);

	ROUND_ATTRIB_TO_INT(StartingHealth);
	ROUND_ATTRIB_TO_INT(ExtraHealthPerLevel);

	ROUND_ATTRIB_TO_INT(Strength);
	ROUND_ATTRIB_TO_INT(Agility);
	ROUND_ATTRIB_TO_INT(Intelligence);
	ROUND_ATTRIB_TO_INT(Stamina);

	// Clamp Mana to their maxes
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
	}

	// Clamp Characteristics to 1+ ints since some math formulas assume they are positive
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

void ULordHeroAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ROUND_ATTRIB_TO_INT(MaxMana);
	ROUND_ATTRIB_TO_INT(Mana);

	ROUND_ATTRIB_TO_INT(StartingHealth);
	ROUND_ATTRIB_TO_INT(ExtraHealthPerLevel);

	ROUND_ATTRIB_TO_INT(Strength);
	ROUND_ATTRIB_TO_INT(Agility);
	ROUND_ATTRIB_TO_INT(Intelligence);
	ROUND_ATTRIB_TO_INT(Stamina);
}

void ULordHeroAttributeSet::Init(ULordUnitAttributeSet* InUnitAttribs)
{
	ensure(!UnitAttribs.IsValid());
	UnitAttribs = InUnitAttribs;
}

void ULordHeroAttributeSet::UpdateDerivedUnitValues()
{
	// Health and Mana are based on Stamina and Intelligence...
	// Proc the 'post attribute change' for these
	PostAttributeChange(GetStaminaAttribute(), 0, GetStamina());
	PostAttributeChange(GetIntelligenceAttribute(), 0, GetIntelligence());
}

void ULordHeroAttributeSet::UpdateAfterLevelup()
{
	ResetBaseHealth();
	ResetBaseMana();
}

int ULordHeroAttributeSet::CalculateBaseHealth() const
{
	const float StaminaValue = GetStamina();
	return GetStartingHealth() // Base amount
		+ FMath::RoundToInt(StaminaValue * 3.5)
		+ ((StaminaValue + GetExtraHealthPerLevel()) * (UnitAttribs->GetLevel() - 1)) // Amount-per-level (stamina + bonus) * number of level ups
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
