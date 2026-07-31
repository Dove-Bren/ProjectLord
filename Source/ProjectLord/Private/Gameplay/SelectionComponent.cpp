// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionComponent.h"

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
}

FSelectionData USelectionComponent::Select()
{
	OnSelected.Broadcast();

	FSelectionData SelectData(this);
	SelectData.AvailableActions.Append(ActionInstances);

	return SelectData;
}

void USelectionComponent::Deselect()
{
	OnDeselected.Broadcast();
}
