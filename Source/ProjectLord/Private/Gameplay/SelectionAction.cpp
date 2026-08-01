// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionAction.h"

#include "Gameplay/LordPlayerState.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Units/Unit.h"
#include "Gameplay/Buildings/Building.h"

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

	// TODO #41 check the queue, too!

	return !BuildingOwner->HasGood(GetGood().Good);
}

bool UResearchGoodPurchase::Perform_Implementation(const FSelectionActionContext& Context)
{
	auto Building = GetBuilding(Context);
	if (!ensure(Building))
	{
		return false;
	}

	Context.PlayerState->AddGold(-GetGoldCost());

	// TODO #41 - Building Queues
	//Building->QueueResearchGood(GetGood());

	return true;
}

bool URecruitUnitPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	// TODO #41 - Building Queues
	// Check how many units a building has queued and if the queue size is big enough
	// Make sure to add queued units to existing units, and check that too!

	return Super::CanPerform_Implementation(Context);
}

bool URecruitUnitPurchase::Perform_Implementation(const FSelectionActionContext& Context)
{
	auto Building = GetBuilding(Context);
	if (!ensure(Building))
	{
		return false;
	}

	Context.PlayerState->AddGold(-GetGoldCost());

	// TODO #41 - Building Queues
	//Building->QueueRecruitUnit(GetUnitType());

	return true;
}
