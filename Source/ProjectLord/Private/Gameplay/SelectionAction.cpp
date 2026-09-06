// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionAction.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/GameTeam.h"
#include "Gameplay/LordPlayerController.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Units/RewardFlag.h"
#include "Gameplay/Units/Unit.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/BuildingTypes.h"
#include "Gameplay/Buildings/GoodBuilding.h"
#include "Gameplay/Buildings/BuildingActionQueue.h"
#include "UI/ViewModels/SelectionActionViewModel.h"

void USelectionAction::Setup(const FSelectionActionContext& InContext)
{
	Context = InContext;

	ViewModel = UVMSelectionAction::Make(this, this);
	ViewModel->SetEnabled(CanPerform());
	ViewModel->SetHidden(IsHidden());
}

bool USelectionAction::IsHidden_Implementation() const
{
	return false;
}

bool USelectionAction::CanPerform_Implementation() const
{
	ensureMsgf(false, TEXT("Selection Action did not implement CanPerform"));
	return false;
}

bool USelectionAction::Perform_Implementation()
{
	ensureMsgf(false, TEXT("Selection Action did not implement Perform"));
	return false;
}

void USelectionAction::RefreshToShow_Implementation()
{
	; // Nothing that needs doing
}

void USelectionPurchase::Setup(const FSelectionActionContext& InContext)
{
	Super::Setup(InContext);

	if (ALordPlayerController* PC = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		ViewModel->SetEnabled(CanPerform());
		if (ensure(Context.TeamState))
		{
			Context.TeamState->OnTeamGoldChanged.AddWeakLambda(this, [this, InContext](int Gold)
				{
					ViewModel->SetEnabled(CanPerform());
				});
		}
	}
}

bool USelectionPurchase::CanPerform_Implementation() const
{
	return Context.TeamState->GetGold() >= GetGoldCost();
}

bool USelectionPurchase::DeductGoldCost()
{
	const int Cost = GetGoldCost();
	if (Context.TeamState && Context.TeamState->GetGold() >= Cost)
	{
		Context.TeamState->AddGold(-Cost);
		return true;
	}
	return false;
}

AUnit* UUnitBasedPurchase::GetUnitInner() const
{
	return Cast<AUnit>(Context.Selection->GetOwner());
}

AUnit* UUnitBasedPurchase::GetUnit() const
{
	return GetUnitInner();
}

void UUnitBasedPurchase::Setup(const FSelectionActionContext& InContext)
{
	Super::Setup(InContext);

	AUnit* UnitOwner = GetUnitInner();
}

bool UUnitBasedPurchase::CanPerform_Implementation() const
{
	// Must have a unit performing us
	AUnit* UnitOwner = GetUnit();
	if (!IsValid(UnitOwner))
	{
		return false;
	}

	return Super::CanPerform_Implementation();
}

ARewardFlag* UFlagBasedPurchase::GetFlagInner() const
{
	return Cast<ARewardFlag>(Context.Selection->GetOwner());
}

ARewardFlag* UFlagBasedPurchase::GetFlag() const
{
	return GetFlagInner();
}

void UFlagBasedPurchase::Setup(const FSelectionActionContext& InContext)
{
	Super::Setup(InContext);

	ARewardFlag* FlagOwner = GetFlagInner();
	//TODO:
	// FlagOwner->OnDeath.AddWeakLambda()...
}

bool UFlagBasedPurchase::CanPerform_Implementation() const
{
	// Must have a unit performing us
	ARewardFlag* FlagOwner = GetFlag();
	if (!IsValid(FlagOwner))
	{
		return false;
	}

	return Super::CanPerform_Implementation();
}

ABuilding* UBuildingBasedPurchase::GetBuildingInner() const
{
	auto ActionOwner = Context.Selection->GetOwner();
	return Cast<ABuilding>(ActionOwner);
}

ABuilding* UBuildingBasedPurchase::GetBuilding() const
{
	return GetBuildingInner();
}

void UBuildingBasedPurchase::Setup(const FSelectionActionContext& InContext)
{
	Super::Setup(InContext);

	auto BuildingOwner = GetBuildingInner();
	if (ensure(BuildingOwner))
	{
		// TODO building destruction
		BuildingOwner->OnBuildingLevelChanged.AddWeakLambda(this, [this, InContext](int NewLevel) {
			ViewModel->SetEnabled(CanPerform());
		});
		BuildingOwner->OnBuildingHealthChanged.AddWeakLambda(this, [this, InContext](int Health, int MaxHealth) {
			ViewModel->SetEnabled(CanPerform());
			});
	}
}

bool UBuildingBasedPurchase::CanPerform_Implementation() const
{
	// Must have a building performing us
	ABuilding* BuildingOwner = GetBuilding();
	if (!IsValid(BuildingOwner))
	{
		return false;
	}

	// Check building level requirement
	if (BuildingOwner->GetBuildingLevel() < RequiredBuildingLevel)
	{
		return false;
	}

	return Super::CanPerform_Implementation();
}

void UResearchGoodPurchase::Setup(const FSelectionActionContext& InContext)
{
	Super::Setup(InContext);

	auto BuildingOwner = GetBuildingInner();
	auto GoodBuilding = Cast<AGoodBuilding>(BuildingOwner);
	if (ensure(GoodBuilding))
	{
		GoodBuilding->OnBuildingGoodsChanged.AddWeakLambda(this, [this, InContext]()
		{
			ViewModel->SetHidden(IsHidden());
		});
	}
}

bool UResearchGoodPurchase::CanPerform_Implementation() const
{
	if (!Super::CanPerform_Implementation())
	{
		return false;
	}

	if (IsHidden())
	{
		return false;
	}

	// Check if building has good already
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding());
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

bool UResearchGoodPurchase::IsHidden_Implementation() const
{
	// Check if building has good already
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding());
	if (!ensure(IsValid(BuildingOwner)))
	{
		return false;
	}

	return BuildingOwner->HasGood(GetGood().Good);
}

bool UResearchGoodPurchase::Perform_Implementation()
{
	AGoodBuilding* Building = Cast<AGoodBuilding>(GetBuilding());
	if (!ensure(Building))
	{
		return false;
	}
	auto Queue = Building->GetQueueComponent();

	auto Action = NewObject<UQueuedGoodAction>(Queue);
	Action->Init(GetGoldCost(), GetGood());

	DeductGoldCost();
	Queue->QueueAction(Action);

	return true;
}

bool UPlaceBuildingPurchase::CanPerform_Implementation() const
{
	// TODO: check building requirements

	if (!ensure(IsValid(BuildingType)))
	{
		return false;
	}

	return Super::CanPerform_Implementation();
}

bool UPlaceBuildingPurchase::Perform_Implementation()
{
	ALordPlayerController* Controller = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!Controller)
	{
		return false;
	}

	Controller->PlaceBuilding(GetBuildingType(), GetGoldCost());
	return true;
}

void URecruitUnitPurchase::Setup(const FSelectionActionContext& InContext)
{
	Super::Setup(InContext);

	auto BuildingOwner = GetBuildingInner();
	if (ensure(BuildingOwner))
	{
		// TODO building unit counts changed event
	}
}

bool URecruitUnitPurchase::CanPerform_Implementation() const
{
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding());
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

	return Super::CanPerform_Implementation();
}

bool URecruitUnitPurchase::Perform_Implementation()
{
	AGoodBuilding* Building= Cast<AGoodBuilding>(GetBuilding());
	if (!ensure(Building))
	{
		return false;
	}

	auto Queue = Building->GetQueueComponent();

	auto Action = NewObject<UQueuedRecruitAction>(Queue);
	Action->Init(GetGoldCost(), GetUnitType());

	DeductGoldCost();
	Queue->QueueAction(Action);

	return true;
}
