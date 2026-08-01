// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionAction.h"

#include "Gameplay/LordPlayerState.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Units/Unit.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/BuildingActionQueue.h"

bool USelectionAction::IsHidden_Implementation(const FSelectionActionContext& Context) const
{
	return false;
}

bool USelectionAction::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	ensureMsgf(false, TEXT("Selection Action did not implement CanPerform"));
	return false;
}

bool USelectionAction::Perform_Implementation(const FSelectionActionContext& Context)
{
	ensureMsgf(false, TEXT("Selection Action did not implement Perform"));
	return false;
}

bool USelectionPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	return Context.PlayerState->GetPlayerGold() >= GetGoldCost();
}

AUnit* UUnitBasedPurchase::GetUnit(const FSelectionActionContext& Context) const
{
	auto ActionOwner = Context.Selection->GetOwner();
	return Cast<AUnit>(ActionOwner);
}

bool UUnitBasedPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	// Must have a unit performing us
	AUnit* UnitOwner = GetUnit(Context);
	if (!ensure(IsValid(UnitOwner)))
	{
		return false;
	}

	return Super::CanPerform_Implementation(Context);
}

ABuilding* UBuildingBasedPurchase::GetBuilding(const FSelectionActionContext& Context) const
{
	auto ActionOwner = Context.Selection->GetOwner();
	return Cast<ABuilding>(ActionOwner);
}

bool UBuildingBasedPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	// Must have a building performing us
	ABuilding* BuildingOwner = GetBuilding(Context);
	if (!ensure(IsValid(BuildingOwner)))
	{
		return false;
	}

	return Super::CanPerform_Implementation(Context);
}

bool UResearchGoodPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	if (!Super::CanPerform_Implementation(Context))
	{
		return false;
	}

	// Check if building has good already
	ABuilding* BuildingOwner = GetBuilding(Context);
	auto Queue = BuildingOwner->GetQueueComponent();

	if (Queue->IsFull())
	{
		return false;
	}

	return !BuildingOwner->HasGood(GetGood().Good);
}

bool UResearchGoodPurchase::Perform_Implementation(const FSelectionActionContext& Context)
{
	auto Building = GetBuilding(Context);
	if (!ensure(Building))
	{
		return false;
	}
	auto Queue = Building->GetQueueComponent();

	auto Action = NewObject<UQueuedGoodAction>(Queue);
	Action->Init(GetGoldCost(), GetGood());

	Context.PlayerState->AddGold(-GetGoldCost());
	Queue->QueueAction(Action);

	return true;
}

bool URecruitUnitPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	ABuilding* BuildingOwner = GetBuilding(Context);
	auto Queue = BuildingOwner->GetQueueComponent();

	if (Queue->IsFull())
	{
		return false;
	}

	return Super::CanPerform_Implementation(Context);
}

bool URecruitUnitPurchase::Perform_Implementation(const FSelectionActionContext& Context)
{
	auto Building = GetBuilding(Context);
	if (!ensure(Building))
	{
		return false;
	}

	auto Queue = Building->GetQueueComponent();

	auto Action = NewObject<UQueuedRecruitAction>(Queue);
	Action->Init(GetGoldCost(), GetUnitType());

	Context.PlayerState->AddGold(-GetGoldCost());
	Queue->QueueAction(Action);

	return true;
}
