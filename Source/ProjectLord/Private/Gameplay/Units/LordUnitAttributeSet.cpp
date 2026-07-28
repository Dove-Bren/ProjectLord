// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/LordUnitAttributeSet.h"

#include "Net/UnrealNetwork.h"


void ULordUnitAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULordUnitAttributeSet, Level);
	DOREPLIFETIME(ULordUnitAttributeSet, MaxHealth);
	DOREPLIFETIME(ULordUnitAttributeSet, Health);
	DOREPLIFETIME(ULordUnitAttributeSet, MeleeDefense);
	DOREPLIFETIME(ULordUnitAttributeSet, RangedDefense);
	DOREPLIFETIME(ULordUnitAttributeSet, MagicDefense);

	DOREPLIFETIME(ULordUnitAttributeSet, Movement);

	DOREPLIFETIME(ULordUnitAttributeSet, BonusMeleeDamage);
	DOREPLIFETIME(ULordUnitAttributeSet, BonusRangedDamage);
	DOREPLIFETIME(ULordUnitAttributeSet, BonusMagicDamage);

	DOREPLIFETIME(ULordUnitAttributeSet, WanderRadius);
	DOREPLIFETIME(ULordUnitAttributeSet, Sight);
	DOREPLIFETIME(ULordUnitAttributeSet, AttackRange);
}

void ULordUnitAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void ULordUnitAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ROUND_ATTRIB_TO_INT(Level);
	ROUND_ATTRIB_TO_INT(MaxHealth);
	ROUND_ATTRIB_TO_INT(Health);

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
}

void ULordUnitAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
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
}

void ULordUnitAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ROUND_ATTRIB_TO_INT(Level);
	ROUND_ATTRIB_TO_INT(MaxHealth);
	ROUND_ATTRIB_TO_INT(Health);
	ROUND_ATTRIB_TO_INT(MeleeDefense);
	ROUND_ATTRIB_TO_INT(RangedDefense);
	ROUND_ATTRIB_TO_INT(MagicDefense);

	ROUND_ATTRIB_TO_INT(BonusMeleeDamage);
	ROUND_ATTRIB_TO_INT(BonusRangedDamage);
	ROUND_ATTRIB_TO_INT(BonusMagicDamage);
}