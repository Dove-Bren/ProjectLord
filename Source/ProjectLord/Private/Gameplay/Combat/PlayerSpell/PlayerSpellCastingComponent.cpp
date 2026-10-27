// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/PlayerSpell/PlayerSpellCastingComponent.h"

#include "Gameplay/GameTeamState.h"
#include "Gameplay/LordAbilitySystemComponent.h"
#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpell.h"
#include "UI/ViewModels/PlayerSpellCastingViewModel.h"

UPlayerSpellCastingComponent::UPlayerSpellCastingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPlayerSpellCastingComponent::BeginPlay()
{
	Super::BeginPlay();

	ViewModel = CreateLordVM<UVMPlayerSpellCasting>(this);
}

void UPlayerSpellCastingComponent::StartCasting(UPlayerSpell* Spell)
{
	CurrentSpell = Spell;
	GetPlayerController()->SetCursorMode(EMouseCursor::Crosshairs);
	OnSpellChanged.Broadcast();
}

void UPlayerSpellCastingComponent::CancelCasting()
{
	CurrentSpell = nullptr;
	GetPlayerController()->SetCursorMode(EMouseCursor::Default);
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

TOptional<FPlayerSpellCastOperation> UPlayerSpellCastingComponent::AttemptToCast()
{
	if (!IsCasting())
	{
		return NullOpt;
	}

	auto PC = GetPlayerController();
	if (!PC)
	{
		return NullOpt;
	}
	
	const FVector AtLoc = PC->GetWorldPositionUnderMouse();
	const auto AtComponent = PC->GetCombatUnderMouse();

	if (!CurrentSpell->CanCast(PC, AtLoc, AtComponent))
	{
		return NullOpt;
	}

	// Now I'm wondering if this should be here...
	OnSpellCast.Broadcast(CurrentSpell);

	FPlayerSpellCastOperation Operation = {};
	Operation.Spell = CurrentSpell;
	Operation.TargetComponent = AtComponent;
	Operation.TargetLocation = AtLoc;
	return Operation;
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

bool UPlayerSpellCastingComponent::CanAfford(int CurrentCost) const
{
	auto PC = Cast<ALordPlayerController>(GetOwner());
	if (PC)
	{
		return PC->GetTeamState()->GetGold() >= CurrentCost;
	}
	return false;
}

void UPlayerSpellCastingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsCasting())
	{
		const int Cost = GetCastCost();
		const bool bCanAfford = CanAfford(Cost);
		const bool bIsAllowed = bCanAfford && CanCast();
		ViewModel->SetCurrentCost(Cost);
		ViewModel->SetCanAfford(bCanAfford);
		ViewModel->SetCanCast(bIsAllowed);
	}
}
