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

	if (ALordPlayerController* PC = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(Outer, 0)))
	{
		FSelectionActionContext Context = PC->MakeSelectionContext();
		VM->SetEnabled(Action->CanPerform(Context));
		Context.PlayerState->OnPlayerGoldChanged.AddWeakLambda(VM, [Context, VM, Action](int Gold)
			{
				VM->SetEnabled(Action->CanPerform(Context));
			});
	}
	
	VM->Action = Action;

	return VM;
}

void UVMSelectionAction::Perform()
{
	if (!ensure(Action.IsValid()))
	{
		return;
	}

	ALordPlayerController* PC = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(Action.Get(), 0));
	if (ensure(PC))
	{
		FSelectionActionContext Context = PC->MakeSelectionContext();
		Action.Get()->Perform(Context);
	}
}