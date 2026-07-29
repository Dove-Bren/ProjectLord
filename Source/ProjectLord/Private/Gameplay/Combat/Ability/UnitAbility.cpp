// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/UnitAbility.h"

#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/AI/UnitController.h"

UCombatComponent* UUnitAbility::GetOwnerComponent() const
{
	AActor* Owner = GetOwningActorFromActorInfo();
	return Owner->GetComponentByClass<UCombatComponent>();
}

UCombatComponent* UUnitAbility::GetOwnerTarget(bool bOnlyAlive) const
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

