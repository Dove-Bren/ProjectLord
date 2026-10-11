// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/BuildingActionQueue.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/Buildings/Building.h"
#include "UI/ViewModels/Generic/ProgressQueueViewModel.h"

UBuildingActionQueueComponent::UBuildingActionQueueComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UBuildingActionQueueComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Queue);
	DOREPLIFETIME(ThisClass, TimeRemaining);
}

void UBuildingActionQueueComponent::BeginPlay()
{
	Super::BeginPlay();

	ViewModel = CreateLordVM<UVMProgressQueue>(this);
	ViewModel->SetShowQueueActions(true);
}

void UBuildingActionQueueComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasActionInProgress())
	{
		TimeRemaining -= DeltaTime;
		if (TimeRemaining <= 0)
		{
			auto Action = Queue[0]; // Copy
			Queue.RemoveAt(0, EAllowShrinking::No);
			PerformAction(Action);
			if (Action.Execution)
			{
				RemoveReplicatedSubObject(Action.Execution);
			}
			ResetTimer();
			NotifyQueueChange();
		}

		ViewModel->SetProgress(GetProgress());
	}
}

float UBuildingActionQueueComponent::GetProgress() const
{
	if (!HasActionInProgress())
	{
		return 0.0f;
	}

	auto Action = GetActiveAction();
	return 1.0f - (TimeRemaining / GetTotalTimeFor(Action));
}

bool UBuildingActionQueueComponent::HasActionInProgress() const
{
	return !Queue.IsEmpty();
}

bool UBuildingActionQueueComponent::IsFull() const
{
	return Queue.Num() >= 4;
}

void UBuildingActionQueueComponent::QueueAction(FQueuedAction InAction)
{
	if (InAction.Execution)
	{
		AddReplicatedSubObject(InAction.Execution);
	}
	Queue.Add(MoveTemp(InAction));
	if (Queue.Num() == 1)
	{
		ResetTimer();
	}
	NotifyQueueChange();
}

void UBuildingActionQueueComponent::ResetTimer()
{
	if (!Queue.IsEmpty())
	{
		TimeRemaining = GetTotalTimeFor(GetActiveAction());
	}
}

float UBuildingActionQueueComponent::GetTotalTimeFor(const FQueuedAction* Action) const
{
	return 10.0f; // HARDCODED TIME
}

const FQueuedAction* UBuildingActionQueueComponent::GetActiveAction() const
{
	return Queue.IsEmpty() ? nullptr : &Queue[0];
}

void UBuildingActionQueueComponent::PerformAction(FQueuedAction& Action)
{
	OnActionReady.Broadcast(Action);
}

void UBuildingActionQueueComponent::NotifyQueueChange()
{
	// Update VM
	if (ViewModel)
	{
		ViewModel->UpdateQueue(Queue);
	}
	OnQueueChange.Broadcast();
}

void UBuildingActionQueueComponent::OnRep_Queue()
{
	NotifyQueueChange();
}

void UBuildingActionQueueComponent::OnRep_TimeRemaining()
{

}
