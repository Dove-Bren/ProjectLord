// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionComponent.h"

USelectionComponent::USelectionComponent()
{
	bSelectable = true;

	PrimaryComponentTick.bCanEverTick = false;
}

FSelectionData USelectionComponent::Select()
{
	OnSelected.Broadcast();

	return FSelectionData(this);
}

void USelectionComponent::Deselect()
{
	OnDeselected.Broadcast();
}
