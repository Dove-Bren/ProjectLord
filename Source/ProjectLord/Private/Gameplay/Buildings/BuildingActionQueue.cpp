// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/BuildingActionQueue.h"

#include "Gameplay/Buildings/Building.h"

UBuildingActionQueueComponent::UBuildingActionQueueComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuildingActionQueueComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasActionInProgress())
	{
		TimeRemaining -= DeltaTime;
		if (TimeRemaining <= 0)
		{
			auto* Action = Queue[0];
			Queue.RemoveAt(0, EAllowShrinking::No);
			PerformAction(Action);
			ResetTimer();
		}
	}
}

float UBuildingActionQueueComponent::GetProgress() const
{
	return 0;
}

bool UBuildingActionQueueComponent::HasActionInProgress() const
{
	return !Queue.IsEmpty();
}

bool UBuildingActionQueueComponent::IsFull() const
{
	return Queue.Num() >= 4;
}

void UBuildingActionQueueComponent::QueueAction(UQueuedAction* InAction)
{
	Queue.Add(InAction);
	if (Queue.Num() == 1)
	{
		ResetTimer();
	}
}

void UBuildingActionQueueComponent::ResetTimer()
{
	if (!Queue.IsEmpty())
	{
		TimeRemaining = GetTotalTimeFor(GetActiveAction());
	}
}

float UBuildingActionQueueComponent::GetTotalTimeFor(const UQueuedAction* Action) const
{
	return 10.0f; // HARDCODED TIME
}

UQueuedAction* UBuildingActionQueueComponent::GetActiveAction() const
{
	return Queue.IsEmpty() ? nullptr : Queue[0];
}

void UBuildingActionQueueComponent::PerformAction(UQueuedAction* Action)
{
	OnActionReady.Broadcast(Action);
}
