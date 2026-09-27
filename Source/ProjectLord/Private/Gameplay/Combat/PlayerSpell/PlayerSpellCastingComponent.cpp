// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/PlayerSpell/PlayerSpellCastingComponent.h"

#include "AbilitySystemComponent.h"

#include "Gameplay/LordPlayerController.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpell.h"

UPlayerSpellCastingComponent::UPlayerSpellCastingComponent()
{
}

void UPlayerSpellCastingComponent::StartCasting(UPlayerSpell* Spell)
{
	CurrentSpell = Spell;
	OnSpellChanged.Broadcast();
}

void UPlayerSpellCastingComponent::CancelCasting()
{
	CurrentSpell = nullptr;
	OnSpellChanged.Broadcast();
}

bool UPlayerSpellCastingComponent::CanCast() const
{
	if (!IsCasting())
	{
		return false;
	}

	auto PC = GetPlayerController();
	if (!PC)
	{
		return false;
	}

	return CurrentSpell->CanCast(PC, PC->GetWorldPositionUnderMouse(), PC->GetCombatUnderMouse());
}

bool UPlayerSpellCastingComponent::AttemptToCast()
{
	if (!IsCasting())
	{
		return false;
	}

	auto PC = GetPlayerController();
	if (!PC)
	{
		return false;
	}

	if (!CurrentSpell->AttemptCast(PC, PC->GetWorldPositionUnderMouse(), PC->GetCombatUnderMouse()))
	{
		return false;
	}

	OnSpellCast.Broadcast(CurrentSpell);
	return true;
}

int UPlayerSpellCastingComponent::GetCastCost() const
{
	if (!IsCasting())
	{
		return 0;
	}

	auto PC = GetPlayerController();
	if (!PC)
	{
		return 0;
	}

	return CurrentSpell->GetGoldCostAt(PC, PC->GetWorldPositionUnderMouse());
}

ALordPlayerController* UPlayerSpellCastingComponent::GetPlayerController() const
{
	auto PC = Cast<ALordPlayerController>(GetOwner());
	ensure(PC);
	return PC;
}
