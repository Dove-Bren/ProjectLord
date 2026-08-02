// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Attributes/CombatAttributeSet.h"

#include "Net/UnrealNetwork.h"


void UCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatAttributeSet, Level);
	DOREPLIFETIME(UCombatAttributeSet, MaxHealth);
	DOREPLIFETIME(UCombatAttributeSet, Health);

	DOREPLIFETIME(UCombatAttributeSet, MaxMana);
	DOREPLIFETIME(UCombatAttributeSet, Mana);
	DOREPLIFETIME(UCombatAttributeSet, MeleeDefense);
	DOREPLIFETIME(UCombatAttributeSet, RangedDefense);
	DOREPLIFETIME(UCombatAttributeSet, MagicDefense);

	DOREPLIFETIME(UCombatAttributeSet, BonusMeleeDamage);
	DOREPLIFETIME(UCombatAttributeSet, BonusRangedDamage);
	DOREPLIFETIME(UCombatAttributeSet, BonusMagicDamage);

	DOREPLIFETIME(UCombatAttributeSet, Sight);
	DOREPLIFETIME(UCombatAttributeSet, AttackRange);
}

void UCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ROUND_ATTRIB_TO_INT(Level);
	ROUND_ATTRIB_TO_INT(MaxHealth);
	ROUND_ATTRIB_TO_INT(Health);

	ROUND_ATTRIB_TO_INT(MaxMana);
	ROUND_ATTRIB_TO_INT(Mana);

	// Maybe these shouldn't round their actual value, and callers should have to round.
	// That way, if you have 10 MeleeDefense and get a 15% buff you can have 11.5 in reality, and effectively 11 or 12
	/*
	ROUND_ATTRIB_TO_INT(MeleeDefense);
	ROUND_ATTRIB_TO_INT(RangedDefense);
	ROUND_ATTRIB_TO_INT(MagicDefense);

	ROUND_ATTRIB_TO_INT(BonusMeleeDamage);
	ROUND_ATTRIB_TO_INT(BonusRangedDamage);
	ROUND_ATTRIB_TO_INT(BonusMagicDamage);*/

	// Clamp Health to max
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}

	// Clamp Mana to their maxes
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
	}
}

void UCombatAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// If Max Health changes, make sure to cap health
	if (Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() < GetMaxHealth())
		{
			SetHealth(GetMaxHealth());
		}
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

void UCombatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ROUND_ATTRIB_TO_INT(Level);
	ROUND_ATTRIB_TO_INT(MaxHealth);
	ROUND_ATTRIB_TO_INT(Health);
	ROUND_ATTRIB_TO_INT(MaxMana);
	ROUND_ATTRIB_TO_INT(Mana);
	ROUND_ATTRIB_TO_INT(MeleeDefense);
	ROUND_ATTRIB_TO_INT(RangedDefense);
	ROUND_ATTRIB_TO_INT(MagicDefense);

	ROUND_ATTRIB_TO_INT(BonusMeleeDamage);
	ROUND_ATTRIB_TO_INT(BonusRangedDamage);
	ROUND_ATTRIB_TO_INT(BonusMagicDamage);
}