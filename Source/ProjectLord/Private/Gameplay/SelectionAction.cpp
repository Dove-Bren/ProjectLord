// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionAction.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/GameTeam.h"
#include "Gameplay/LordPlayerController.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Units/Unit.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/BuildingTypes.h"
#include "Gameplay/Buildings/GoodBuilding.h"
#include "Gameplay/Buildings/BuildingActionQueue.h"
#include "UI/ViewModels/SelectionActionViewModel.h"

void USelectionAction::Setup(const FSelectionActionContext& Context)
{
	ViewModel = UVMSelectionAction::Make(this, this);
	ViewModel->SetEnabled(CanPerform(Context));
}

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

void USelectionPurchase::Setup(const FSelectionActionContext& Context)
{
	Super::Setup(Context);

	if (ALordPlayerController* PC = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		ViewModel->SetEnabled(CanPerform(Context));
		if (ensure(Context.TeamState))
		{
			Context.TeamState->OnTeamGoldChanged.AddWeakLambda(this, [this, Context](int Gold)
				{
					ViewModel->SetEnabled(CanPerform(Context));
				});
		}
	}
}

bool USelectionPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	return Context.TeamState->GetGold() >= GetGoldCost();
}

bool USelectionPurchase::DeductGoldCost(const FSelectionActionContext& Context)
{
	const int Cost = GetGoldCost();
	if (Context.TeamState && Context.TeamState->GetGold() >= Cost)
	{
		Context.TeamState->AddGold(-Cost);
		return true;
	}
	return false;
}

AUnit* UUnitBasedPurchase::GetUnitInner(const FSelectionActionContext& Context) const
{
	return Cast<AUnit>(Context.Selection->GetOwner());
}

AUnit* UUnitBasedPurchase::GetUnit(const FSelectionActionContext& Context) const
{
	return GetUnitInner(Context);
}

void UUnitBasedPurchase::Setup(const FSelectionActionContext& Context)
{
	Super::Setup(Context);

	AUnit* UnitOwner = GetUnitInner(Context);
	//TODO:
	// UnitOwner->OnDeath.AddWeakLambda()...
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

ABuilding* UBuildingBasedPurchase::GetBuildingInner(const FSelectionActionContext& Context) const
{
	auto ActionOwner = Context.Selection->GetOwner();
	return Cast<ABuilding>(ActionOwner);
}

ABuilding* UBuildingBasedPurchase::GetBuilding(const FSelectionActionContext& Context) const
{
	return GetBuildingInner(Context);
}

void UBuildingBasedPurchase::Setup(const FSelectionActionContext& Context)
{
	Super::Setup(Context);

	auto BuildingOwner = GetBuildingInner(Context);
	if (ensure(BuildingOwner))
	{
		// TODO building destruction
		BuildingOwner->OnBuildingLevelChanged.AddWeakLambda(this, [this, Context](int NewLevel) {
			ViewModel->SetEnabled(CanPerform(Context));
		});
		BuildingOwner->OnBuildingHealthChanged.AddWeakLambda(this, [this, Context](int Health, int MaxHealth) {
			ViewModel->SetEnabled(CanPerform(Context));
			});
	}
}

bool UBuildingBasedPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	// Must have a building performing us
	ABuilding* BuildingOwner = GetBuilding(Context);
	if (!ensure(IsValid(BuildingOwner)))
	{
		return false;
	}

	// Check building level requirement
	if (BuildingOwner->GetBuildingLevel() < RequiredBuildingLevel)
	{
		return false;
	}

	return Super::CanPerform_Implementation(Context);
}

void UResearchGoodPurchase::Setup(const FSelectionActionContext& Context)
{
	Super::Setup(Context);

	auto BuildingOwner = GetBuildingInner(Context);
	if (ensure(BuildingOwner))
	{
		// TODO building goods event
	}
}

bool UResearchGoodPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	if (!Super::CanPerform_Implementation(Context))
	{
		return false;
	}

	if (IsHidden(Context))
	{
		return false;
	}

	// Check if building has good already
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding(Context));
	if (!ensure(IsValid(BuildingOwner)))
	{
		return false;
	}

	auto Queue = BuildingOwner->GetQueueComponent();

	if (Queue->IsFull())
	{
		return false;
	}

	return true;
}

bool UResearchGoodPurchase::IsHidden_Implementation(const FSelectionActionContext& Context) const
{
	// Check if building has good already
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding(Context));
	if (!ensure(IsValid(BuildingOwner)))
	{
		return false;
	}

	return BuildingOwner->HasGood(GetGood().Good);
}

bool UResearchGoodPurchase::Perform_Implementation(const FSelectionActionContext& Context)
{
	AGoodBuilding* Building = Cast<AGoodBuilding>(GetBuilding(Context));
	if (!ensure(Building))
	{
		return false;
	}
	auto Queue = Building->GetQueueComponent();

	auto Action = NewObject<UQueuedGoodAction>(Queue);
	Action->Init(GetGoldCost(), GetGood());

	DeductGoldCost(Context);
	Queue->QueueAction(Action);

	return true;
}

bool UPlaceBuildingPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	// TODO: check building requirements

	if (!ensure(IsValid(BuildingType)))
	{
		return false;
	}

	return Super::CanPerform_Implementation(Context);
}

bool UPlaceBuildingPurchase::Perform_Implementation(const FSelectionActionContext& Context)
{
	ALordPlayerController* Controller = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!Controller)
	{
		return false;
	}

	Controller->PlaceBuilding(GetBuildingType(), GetGoldCost());
	return true;
}

void URecruitUnitPurchase::Setup(const FSelectionActionContext& Context)
{
	Super::Setup(Context);

	auto BuildingOwner = GetBuildingInner(Context);
	if (ensure(BuildingOwner))
	{
		// TODO building unit counts changed event
	}
}

bool URecruitUnitPurchase::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding(Context));
	if (!ensure(IsValid(BuildingOwner)))
	{
		return false;
	}

	if (!BuildingOwner->CanFitResidentType(GetUnitType()))
	{
		return false;
	}
	
	auto Queue = BuildingOwner->GetQueueComponent();

	if (Queue->IsFull())
	{
		return false;
	}

	return Super::CanPerform_Implementation(Context);
}

bool URecruitUnitPurchase::Perform_Implementation(const FSelectionActionContext& Context)
{
	AGoodBuilding* Building= Cast<AGoodBuilding>(GetBuilding(Context));
	if (!ensure(Building))
	{
		return false;
	}

	auto Queue = Building->GetQueueComponent();

	auto Action = NewObject<UQueuedRecruitAction>(Queue);
	Action->Init(GetGoldCost(), GetUnitType());

	DeductGoldCost(Context);
	Queue->QueueAction(Action);

	return true;
}
