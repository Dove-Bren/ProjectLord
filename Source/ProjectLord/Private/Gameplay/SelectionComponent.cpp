// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/SelectionComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/SelectionAction.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/LordPlayerState.h"
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
	// Reset action tree
	ActionTreeVM->GoToRoot();

	OnSelected.Broadcast();
}

void USelectionComponent::Deselect()
{
	OnDeselected.Broadcast();
}

void USelectionComponent::SetTeam(EGameTeam InTeam)
{
	Team = InTeam;

	// Actions bind to things driven from the team, so regenerate
	ActionTreeVM = BuildDefaultActionTree();
}

UVMSelectionActionTree* USelectionComponent::BuildDefaultActionTree()
{
	UVMSelectionActionTree* VM = CreateLordVM<UVMSelectionActionTree>(this);
	auto GameState = Cast<ALordGameState>(UGameplayStatics::GetGameState(this));
	auto RawState = UGameplayStatics::GetPlayerState(this, 0);
	FSelectionActionContext Context;
	Context.Selection = this;
	Context.PlayerState = Cast<ALordPlayerState>(RawState);
	Context.TeamState = GameState ? GameState->GetTeam(Team) : nullptr;

	ActionInstances.Empty();

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
				Action->Setup(Context);
				ActionInstances.Add(Action);

				AvailableActionVM = Action->GetViewModel();
			}
			ActionVMs.Add(AvailableActionVM);
		}

		VM->AddPage(Page.Key, ActionVMs);
	}

	return VM;
}
