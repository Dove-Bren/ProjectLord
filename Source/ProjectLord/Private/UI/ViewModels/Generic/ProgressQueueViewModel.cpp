// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Generic/ProgressQueueViewModel.h"

void UVMProgressQueue::UpdateQueue(const TArray<UQueuedAction*>& InQueue)
{
	bool bAnyChanges = false;

	// For every one in real action queue, validate or create
	// a matching one in the output queue
	for (int i = 0; i < InQueue.Num(); i++)
	{
		const auto& Action = InQueue[i];
		if (Queue.Num() <= (i)
			|| !Queue[i]->Matches(Action))
		{
			// Either nothing there, or doesn't match
			bAnyChanges = true;
			auto NewVM = UVMQueuedAction::Make(this, Action);
			if (Queue.Num() <= i)
			{
				Queue.Add(NewVM);
			}
			else
			{
				Queue[i] = NewVM;
			}
		}
	}

	// Then drop any that may be in queue that aren't in InQueue anymore
	if (InQueue.Num() < Queue.Num())
	{
		bAnyChanges = true;
		Queue.SetNum(InQueue.Num(), EAllowShrinking::No);
	}

	if (bAnyChanges)
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Queue);
	}
}
