// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/SelectionActionViewModel.h"

#include "Gameplay/SelectionAction.h"
#include "Gameplay/LordPlayerController.h"
#include "Gameplay/LordPlayerState.h"

#include "Kismet/GameplayStatics.h"

/*static*/ UVMSelectionAction* UVMSelectionAction::Make(UObject* Outer, USelectionAction* Action)
{
	auto VM = CreateLordVM<UVMSelectionAction>(Outer);

	VM->SetName(Action->GetName());
	VM->SetDescription(Action->GetDescription());

	if (auto CostAction = Cast<USelectionPurchase>(Action))
	{
		VM->SetGoldCost(CostAction->GetGoldCost());
	}

	VM->SetIcon(Action->GetIcon());
	VM->Action = Action;

	return VM;
}

void UVMSelectionAction::OverrideAction(USelectionAction* NewAction)
{
	Action = NewAction;
}

void UVMSelectionAction::Perform()
{
	if (!ensure(Action.IsValid()))
	{
		return;
	}

	Action.Get()->Perform();
}

void UVMSelectionAction::OnShown()
{
	if (Action.IsValid())
	{
		Action->RefreshToShow();
	}
}