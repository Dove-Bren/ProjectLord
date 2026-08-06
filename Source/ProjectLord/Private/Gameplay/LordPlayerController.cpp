// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "Gameplay/LordPlayerState.h"
#include "UI/ViewModels/SelectionViewModel.h"
#include "UI/ViewModels/SelectionActionViewModel.h"

ALordPlayerController::ALordPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

void ALordPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(StartingInputContext))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(StartingInputContext, 0);
		}
	}

	// Set up VM
	SelectionVM = CreateLordVM<UVMSelection>(this);
}

ALordPlayerState* ALordPlayerController::GetLordPlayerState() const
{
	// In multiplayer, this needs some work as the player state is not guaranteed to be replicated
	// if called early enough.
	return GetPlayerState<ALordPlayerState>();
}

EGameTeam ALordPlayerController::GetTeam() const
{
	const auto State = GetLordPlayerState();
	if (ensure(State))
	{
		return State->GetPlayerTeam();
	}

	return EGameTeam::Player1;
}

void ALordPlayerController::SetSelection(USelectionComponent* InSelection)
{
	ClearSelection(false); // To issue deselect callbacks
	Selection = InSelection;
	InSelection->Select();
	OnSelectionChange();
}

void ALordPlayerController::ClearSelection(bool bBroadcast)
{
	if (HasSelection())
	{
		Selection.GetValue()->Deselect();
	}
	Selection = NullOpt;
	if (bBroadcast)
	{
		OnSelectionChange();
	}
}

FSelectionActionContext ALordPlayerController::MakeSelectionContext()
{
	FSelectionActionContext Context;

	Context.PlayerState = GetLordPlayerState();
	if (HasSelection())
	{
		Context.Selection = Selection.GetValue();
	}

	return Context;
}

void ALordPlayerController::OnMouseClick(bool bRightButton)
{
	if (!bRightButton)
	{
		FHitResult HitResult;
		if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Pawn), true, HitResult)
			&& IsValid(HitResult.GetActor())
			&& CanSelect(HitResult.GetActor())
			)
		{
			// CanSelect was called already, so should have a selection component
			USelectionComponent* ClickedSelection = HitResult.GetActor()->GetComponentByClass<USelectionComponent>();
			if (ensure(ClickedSelection))
			{
				SetSelection(ClickedSelection);
			}

		}
		else
		{
			ClearSelection();
		}
	}
}

bool ALordPlayerController::CanSelect(const AActor* ClickedActor) const
{
	return IsValid(ClickedActor->GetComponentByClass<USelectionComponent>());
}

void ALordPlayerController::OnSelectionChange()
{
	// Update VM
	bool bHasNewData = HasSelection();
	SelectionVM->Reset(!bHasNewData);
	if (bHasNewData)
	{
		auto Selected = GetSelection();

		SelectionVM->SetIcon(Selected->GetIcon());
		SelectionVM->SetSelectionName(Selected->GetName());
		SelectionVM->SetSelectionDescription(Selected->GetDescription());
		SelectionVM->SetTeam(Selected->GetTeam());

		SelectionVM->ActionVM = Selected->GetActionVM();
		SelectionVM->CombatDataVM = Selected->GetCombatDataVM();
		SelectionVM->GoldVM = Selected->GetGoldVM();
		SelectionVM->LevelVM = Selected->GetLevelVM();
		SelectionVM->ProgressQueueVM = Selected->GetQueueVM();
		SelectionVM->SlotsVM = Selected->GetSlotsVM();
		//SelectionVM->TargetVM = Selected->GetTargetVM();

		auto Context = MakeSelectionContext();
		for (auto Action : Selected->GetAvailableActions())
		{
			UVMSelectionAction* ActionVM;
			if (!IsValid(Action) || Action->IsHidden(Context))
			{
				ActionVM = nullptr;
			}
			else
			{
				ActionVM = UVMSelectionAction::Make(this, Action);
			}
			SelectionVM->Actions.Add(ActionVM);
		}

		SelectionVM->TriggerSelectionChange();
	}

	BP_OnSelectionChange();
}
