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
	HoverVM = CreateLordVM<UVMSelection>(this);
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

	InSelection->OnRemoved.AddWeakLambda(this, [this]() {
		ClearSelection(true);
	});

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

void ALordPlayerController::SetHovered(USelectionComponent* InHovered)
{
	if (!InHovered && bHasHoverInfo)
	{
		bHasHoverInfo = false;
		HoveredComponent = NullOpt;
		HoverVM->Reset(true);
		OnHoverChange();
	}
	else if (InHovered && InHovered != HoveredComponent)
	{
		bHasHoverInfo = true;
		HoveredComponent = InHovered;
		HoverVM->SetFromSelection(InHovered, MakeSelectionContext(), false, true);
		OnHoverChange();
	}
}

void ALordPlayerController::SetHoveredStaticElement(FStaticSelection StaticElement)
{
	bHasHoverInfo = true;
	HoveredComponent = NullOpt; // Not based on a component anymore
	HoverVM->SetFromStaticElement(StaticElement);
	OnHoverChange();
}

USelectionComponent* ALordPlayerController::GetSelectableUnderMouse()
{
	FHitResult HitResult;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), true, HitResult)
		&& IsValid(HitResult.GetActor())
		&& CanSelect(HitResult.GetActor())
		)
	{
		// CanSelect was called already, so should have a selection component
		return HitResult.GetActor()->GetComponentByClass<USelectionComponent>();
	}

	return nullptr;
}

void ALordPlayerController::OnMouseClick(bool bRightButton)
{
	if (!bRightButton)
	{
		auto Clicked = GetSelectableUnderMouse();
		if (Clicked)
		{
			SetSelection(Clicked);
		}
		else
		{
			ClearSelection();
		}
	}
}

void ALordPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update hover state
	{
		// Hover can either be a component, or a static set value from the UI.
		// Do some careful edge detection here.
		// Either nothing is hovered, or the last thing hovered with a component.
		if (!bHasHoverInfo || HoveredComponent.IsSet())
		{
			// If either are true, update with any component under the mouse.
			SetHovered(GetSelectableUnderMouse());
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
	SelectionVM->SetFromSelection(HasSelection() ? GetSelection() : nullptr, MakeSelectionContext(), true, true);
	BP_OnSelectionChange();
}

void ALordPlayerController::OnHoverChange()
{
	BP_OnHoverChange();
}
