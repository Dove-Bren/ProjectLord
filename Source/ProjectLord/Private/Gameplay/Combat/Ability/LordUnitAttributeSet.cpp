// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/LordUnitAttributeSet.h"

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