// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionComponent.h"

#include "Gameplay/SelectionAction.h"

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

void USelectionComponent::Select()
{
	OnSelected.Broadcast();
}

void USelectionComponent::Deselect()
{
	OnDeselected.Broadcast();
}
