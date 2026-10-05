// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Interaction/SelectionComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/Interaction/SelectionAction.h"
#include "Gameplay/Player/LordPlayerController.h"
#include "UI/ViewModels/SelectionActionTreeViewModel.h"

USelectionComponent::USelectionComponent()
{
	bSelectable = true;

	PrimaryComponentTick.bCanEverTick = false;
}

void USelectionComponent::BeginPlay()
{
	Super::BeginPlay();

	//ActionTreeVM = BuildDefaultActionTree();

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
	bSelected = true;

	//// Reset action tree
	//ActionTreeVM->GoToRoot();

	//// Refresh actions
	//for (auto Action : ActionInstances)
	//{
	//	Action->RefreshToShow();
	//}

	OnSelected.Broadcast();
}

void USelectionComponent::Deselect()
{
	bSelected = false;
	OnDeselected.Broadcast();
}

void USelectionComponent::Hover()
{
	bHovered = true;
	OnHovered.Broadcast();
}

void USelectionComponent::Unhover()
{
	bHovered = false;
	OnUnhovered.Broadcast();
}

void USelectionComponent::SetTeam(EGameTeam InTeam)
{
	Team = InTeam;
}

UVMSelectionActionTree* USelectionComponent::MakeActionTreeVM(UObject* Outer) const
{
	auto PC = GetWorld()->GetFirstPlayerController<ALordPlayerController>();
	check(PC);

	auto Context = PC->MakeSelectionContext();
	UVMSelectionActionTree* VM = CreateLordVM<UVMSelectionActionTree>(Outer);

	for (auto& Page : ActionTree)
	{
		TArray<UVMSelectionAction*> ActionVMs;

		for (auto& Action : Page.Value.Array)
		{
			UVMSelectionAction* AvailableActionVM;
			
			if (!IsValid(Action))
			{
				AvailableActionVM = nullptr;
			}
			else
			{
				AvailableActionVM = Action->MakeViewModel(Outer, Context);
			}
			ActionVMs.Add(AvailableActionVM);
		}

		VM->AddPage(Page.Key, ActionVMs);
	}

	return VM;
}
