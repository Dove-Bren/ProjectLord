// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "Gameplay/LordGameState.h"
#include "Gameplay/LordPlayerState.h"
#include "Gameplay/PlacementComponent.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/RewardFlag.h"
#include "Gameplay/Units/Unit.h"
#include "UI/ViewModels/SelectionViewModel.h"
#include "UI/ViewModels/SelectionActionViewModel.h"

ALordPlayerController::ALordPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bShouldPerformFullTickWhenPaused = true; // Allows camera to update when paused

	PlacementComponent = CreateDefaultSubobject<UPlacementComponent>("Placement");
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

	// Fix camera blur. Idk why this is on the world...
	if (ensure(GetWorld()))
	{
		GetWorld()->bIsCameraMoveableWhenPaused = true;
	}
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

AGameTeamState* ALordPlayerController::GetTeamState() const
{
	if (auto State = GetWorld()->GetGameState<ALordGameState>())
	{
		return State->GetTeam(GetTeam());
	}
	return nullptr;
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
		Selection.GetValue()->OnRemoved.RemoveAll(this);
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
	Context.TeamState = GetTeamState();
	if (HasSelection())
	{
		Context.Selection = Selection.GetValue();
	}

	return Context;
}

void ALordPlayerController::PlaceBuilding(UBuildingType* Type, int Cost)
{
	PlacementComponent->StartPlacing(Type, Cost);
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

void ALordPlayerController::OnSetPaused(bool bPaused)
{
	;
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

FVector ALordPlayerController::GetWorldPositionUnderMouse()
{
	FHitResult HitResult;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), true, HitResult))
	{
		// CanSelect was called already, so should have a selection component
		return HitResult.Location;
	}

	return {};
}

void ALordPlayerController::OnMouseClick(bool bRightButton)
{
	if (bRightButton)
	{
		if (PlacementComponent->IsPlacing())
		{
			PlacementComponent->CancelPlacing();
		}
		else
		{
			// Flag!
			ERewardFlagType FlagType = ERewardFlagType::Explore;
			FVector At;
			UCombatComponent* AttachComponent = nullptr;
			if (auto Over = GetSelectableUnderMouse())
			{
				// Defend or attack depending on team
				FlagType = (Over->GetTeam() == GetTeam())
					? ERewardFlagType::Defend
					: ERewardFlagType::Attack;
				AttachComponent = Over->GetOwner()->GetComponentByClass<UCombatComponent>();
				At = Over->GetOwner()->GetActorLocation() + FVector(0, 0, 500);
			}
			else
			{
				FlagType = ERewardFlagType::Explore;
				At = GetWorldPositionUnderMouse() + FVector(0, 0, 500);
			}

			ARewardFlag* Flag = ARewardFlag::Make(this, FlagClasses[FlagType], FlagType, At);
			if (AttachComponent)
			{
				Flag->SetAttachedComponent(AttachComponent);
			}
			Flag->SetReward(0);
			Flag->SetTeam(GetTeam());

			GetTeamState()->AddFlag(Flag);
			SetSelection(Flag->GetComponentByClass<USelectionComponent>());
		}
	}
	else
	{
		if (PlacementComponent->IsPlacing())
		{
			if (auto Building = PlacementComponent->AttemptToPlace())
			{
				auto State = GetTeamState();
				if (ensure(State))
				{
					State->AddGold(-PlacementComponent->GetPlaceCost());
				}
				PlacementComponent->CancelPlacing();

				Building->SetTeam(GetTeam());
				Building->HandleBuildingPlacement();
			}
		}
		else
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

bool ALordPlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate)
{
	if (Super::SetPause(bPause, CanUnpauseDelegate))
	{
		// Emit events that can drive UI
		OnSetPaused(bPause);
		BP_OnSetPaused(bPause);

		return true;
	}
	return false;
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
