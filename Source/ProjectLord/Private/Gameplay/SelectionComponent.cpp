// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionComponent.h"

#include "Gameplay/SelectionAction.h"

#include "UI/ViewModels/SelectionViewModel.h"
#include "UI/ViewModels/SelectionActionViewModel.h"
#include "UI/ViewModels/SelectionActionTreeViewModel.h"

USelectionComponent::USelectionComponent()
{
	bSelectable = true;

	PrimaryComponentTick.bCanEverTick = false;
}

void USelectionComponent::BeginPlay()
{
	Super::BeginPlay();

	ActionTreeVM = BuildDefaultActionTree();

	//ViewModel = CreateLordVM<UVMSelection>(this);
	//ViewModel->SetName(GetName(GetOwner()));
}

void USelectionComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	OnRemoved.Broadcast();
}

void USelectionComponent::Select()
{
	OnSelected.Broadcast();
}

void USelectionComponent::Deselect()
{
	OnDeselected.Broadcast();
}

UVMSelectionActionTree* USelectionComponent::BuildDefaultActionTree()
{
	UVMSelectionActionTree* VM = CreateLordVM<UVMSelectionActionTree>(this);

	for (auto& Page : ActionTree)
	{
		TArray<UVMSelectionAction*> ActionVMs;

		for (auto& ActionClass : Page.Value.Array)
		{
			UVMSelectionAction* AvailableActionVM;
			
			if (!IsValid(ActionClass))
			{
				AvailableActionVM = nullptr;
			}
			else
			{
				auto Action = NewObject<USelectionAction>(this, ActionClass);
				AvailableActionVM = UVMSelectionAction::Make(this, Action);
			}
			ActionVMs.Add(AvailableActionVM);
		}

		VM->AddPage(Page.Key, ActionVMs);
	}

	return VM;
}
