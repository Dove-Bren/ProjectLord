// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/UnitAbility.h"

#include "Gameplay/Units/Unit.h"
#include "Gameplay/Units/AI/UnitController.h"

AUnit* UUnitAbility::GetOwnerUnit() const
{
	return Cast<AUnit>(GetOwningActorFromActorInfo());
}

AUnit* UUnitAbility::GetOwnerTarget(bool bOnlyAlive) const
{
	auto Owner = GetOwnerUnit();
	if (!ensure(IsValid(Owner)))
	{
		return nullptr;
	}

	auto OwnerController = Owner->GetUnitController();
	if (!ensure(IsValid(OwnerController)))
	{
		return nullptr;
	}

	auto Target = OwnerController->GetTargetUnit();
	return IsValid(Target) && (!bOnlyAlive || Target->IsAlive())
		? Target
		: nullptr;
}

