// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/SelectionActionViewModel.h"

#include "Gameplay/Interaction/SelectionAction.h"
#include "Gameplay/Player/LordPlayerController.h"

#include "Kismet/GameplayStatics.h"

void UVMSelectionAction::Init(const USelectionAction* InAction)
{
	SetName(Action->GetName());
	SetDescription(Action->GetDescription());

	if (auto CostAction = Cast<USelectionPurchase>(Action))
	{
		SetGoldCost(CostAction->GetGoldCost());
	}

	SetIcon(Action->GetIcon());
	Action = Action;
}

void UVMSelectionAction::SetParentTree(UVMSelectionActionTree* Tree)
{
	ActionTree = Tree;
}

void UVMSelectionAction::OverrideAction(const USelectionAction* NewAction)
{
	Action = NewAction;
}

void UVMSelectionAction::Perform()
{
	if (!ensure(Action.IsValid()))
	{
		return;
	}

	Action.Get()->Perform(GetContext());
}

void UVMSelectionAction::OnShown()
{
	// TODO remove
	/*if (Action.IsValid())
	{
		Action->RefreshToShow();
	}*/
}

FSelectionActionContext UVMSelectionAction::GetContext() const
{
	auto PC = UGameplayStatics::GetPlayerController(this, 0);
	auto LordPC = Cast<ALordPlayerController>(PC);
	if (ensure(LordPC))
	{
		return LordPC->MakeSelectionContext();
	}

	return {};
}