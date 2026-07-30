// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/CombatAbility.h"

#include "Gameplay/Combat/CombatComponent.h"

UCombatComponent* UCombatAbility::GetOwnerComponent() const
{
	AActor* Owner = GetOwningActorFromActorInfo();
	return Owner->GetComponentByClass<UCombatComponent>();
}

UCombatComponent* UCombatAbility::GetOwnerTarget(bool bOnlyAlive) const
{
	auto Owner = GetOwnerComponent();
	if (!ensure(IsValid(Owner)))
	{
		return nullptr;
	}

	auto Target = Owner->GetCombatTarget();
	return IsValid(Target) && (!bOnlyAlive || Target->IsAlive())
		? Target
		: nullptr;
}

