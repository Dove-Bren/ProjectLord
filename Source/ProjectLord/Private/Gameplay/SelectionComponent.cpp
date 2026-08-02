// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionComponent.h"

#include "UI/ViewModels/SelectionViewModel.h"

USelectionComponent::USelectionComponent()
{
	bSelectable = true;

	PrimaryComponentTick.bCanEverTick = false;
}

void USelectionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto& ActionClass : Actions)
	{
		ActionInstances.Add(
			NewObject<USelectionAction>(this, ActionClass)
		);
	}

	//ViewModel = CreateLordVM<UVMSelection>(this);
	//ViewModel->SetName(GetName(GetOwner()));
}

FSelectionData USelectionComponent::Select()
{
	OnSelected.Broadcast();

	FSelectionData SelectData(this);
	SelectData.Name = Name;
	SelectData.Team = Team;
	SelectData.Icon = Icon;
	SelectData.AvailableActions.Append(ActionInstances);

	SelectData.ActionVM = ActionVM;
	SelectData.CombatDataVM = CombatDataVM;
	SelectData.GoldVM = GoldVM;
	SelectData.LevelVM = LevelVM;
	SelectData.QueueVM = QueueVM;
	SelectData.SlotsVM = SlotsVM;

	return SelectData;
}

void USelectionComponent::Deselect()
{
	OnDeselected.Broadcast();
}
