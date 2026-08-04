// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/CombatAbility.h"

#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/Unit.h"

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

void UCombatAbility::ReportAbilityHit(UCombatComponent* HitComponent)
{
	if (!ensure(HitComponent))
	{
		return;
	}

	auto Owner = GetOwnerComponent();
	if (ensure(IsValid(Owner)))
	{
		Owner->NotifyOfAbilityHit(HitComponent);
	}
}

UAnimMontage* UCombatAbility::GetAbilityAnimationFromOwner(EAbilityAnimType Type) const
{
	AActor* Owner = GetOwningActorFromActorInfo();
	if (!ensure(Owner))
	{
		return nullptr;
	}

	AUnit* UnitOwner = Cast<AUnit>(Owner);
	if (!ensure(UnitOwner))
	{
		return nullptr;
	}

	return UnitOwner->GetAnimForAbilityType(Type);
}

