// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Player/LordPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "NavigationSystem.h"
#include "AbilitySystemComponent.h"

#include "LordLogging.h"
#include "Gameplay/GameTeamState.h"
#include "Gameplay/Player/LordCameraPawn.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/Player/LordPlayerState.h"
#include "Gameplay/Player/PlacementComponent.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/Castle.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpellCastingComponent.h"
#include "Gameplay/Units/RewardFlag.h"
#include "Gameplay/Units/Unit.h"
#include "Gameplay/Interaction/SelectionAction.h"
#include "Gameplay/Map/FogOfWarSubsystem.h"
#include "UI/InspectWidget.h"
#include "UI/LordGameHUDWidget.h"
#include "UI/ToastNotification.h"
#include "UI/ViewModels/SelectionViewModel.h"
#include "UI/ViewModels/SelectionActionViewModel.h"

ALordPlayerController::ALordPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bShouldPerformFullTickWhenPaused = true; // Allows camera to update when paused

	PlacementComponent = CreateDefaultSubobject<UPlacementComponent>("Placement");
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	SpellCastingComponent = CreateDefaultSubobject<UPlayerSpellCastingComponent>("PlayerSpellCasting");
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

	const auto State = GetWorld()->GetGameState<ALordGameState>();

	// Rest of initialization needs to wait for playerstate and gamestate
	TArray<TMulticastFuture<void>*> Futures = { &this->PlayerStateReadyVoid, &State->OnGameStateReady, &this->TeamStateReady };
	TMulticastFuture<void>::AllVoid(TDelegate<void()>::CreateWeakLambda(this, [this]() {
		UE_LOG(LogTemp, Display, TEXT("PlayerState and GameState ready"));

		// Set up HUD
		if (IsLocalPlayerController())
		{
			HUDWidget = ConstructHUD();
			check(IsValid(HUDWidget));
			HUDWidget->AddToViewport();
		}

		// TODO: Wait for team states to be set up instead of assuming they are
		auto TeamState = GetTeamState();
		if (ensure(TeamState))
		{
			// TODO need authority check? Auth || isServer or something?
			TeamState->SetPrimaryPlayerController(this);
		}

		// Set up FoW
		if (IsLocalPlayerController())
		{
			auto FogSubsystem = GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
			if (ensure(FogSubsystem))
			{
				FogSubsystem->SetWorldFogForTeam(GetTeam());
			}
		}
	}), Futures);
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

FSelectionActionContext ALordPlayerController::MakeSelectionContext(USelectionComponent* InSelection) const
{
	FSelectionActionContext Context;
	Context.Selection = InSelection ? InSelection : GetSelection();
	Context.PlayerState = GetPlayerState<ALordPlayerState>();
	Context.TeamState = GetTeamState();
	return Context;
}

void ALordPlayerController::PlaceBuilding(UBuildingType* Type, int Cost, const UPlaceBuildingPurchase* Action)
{
	if (SpellCastingComponent->IsCasting()) SpellCastingComponent->CancelCasting();
	PlacementComponent->StartPlacing(Type, Cost, Action);
}

void ALordPlayerController::CastSpell(UPlayerSpell* Spell)
{
	if (PlacementComponent->IsPlacing()) PlacementComponent->CancelPlacing();
	SpellCastingComponent->StartCasting(Spell);
}

bool ALordPlayerController::ShowInspectWidget_Implementation(TSubclassOf<UInspectWidget> WidgetClass, UVMLordBase* VM)
{
	ensureMsgf(false, TEXT("PlayerController needs to implement ShowInspectWidget"));
	return false;
}

void ALordPlayerController::SetFocusedActor_Implementation(const AActor* Actor)
{
	ALordCameraPawn* Camera = Cast<ALordCameraPawn>(GetPawn());
	if (ensure(Camera))
	{
		Camera->SetFocusedActor(Actor);
	}
}

void ALordPlayerController::PanTo_Implementation(FVector WorldPosition)
{
	ALordCameraPawn* Camera = Cast<ALordCameraPawn>(GetPawn());
	if (ensure(Camera))
	{
		Camera->PanTo(WorldPosition);
	}
}

void ALordPlayerController::AddToastNotification(FToastNotification Notification)
{
	if (IsLocalController())
	{
		GetHUDWidget()->AddToastNotification(MoveTemp(Notification));
	}
}

void ALordPlayerController::SetHovered(USelectionComponent* InHovered)
{
	ClearHovered(!IsValid(InHovered)); // Don't broadcast if we're about to update afterwards
	if (IsValid(InHovered))
	{
		bHasHoverInfo = true;
		HoveredComponent = InHovered;
		HoverVM->SetFromSelection(InHovered, false, true);
		HoveredComponent.GetValue()->Hover();
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

void ALordPlayerController::ClearHovered(bool bBroadcast)
{
	if (HasHover())
	{
		if (HoveredComponent.IsSet())
		{
			HoveredComponent.GetValue()->Unhover();
		}
		bHasHoverInfo = false;
		HoveredComponent = NullOpt;
		HoverVM->Reset(bBroadcast);

		if (bBroadcast)
		{
			OnHoverChange();
		}
	}
}

void ALordPlayerController::OnSetPaused(bool bPaused)
{
	;
}

USelectionComponent* ALordPlayerController::GetSelectableUnderMouse() const
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

UCombatComponent* ALordPlayerController::GetCombatUnderMouse() const
{
	FHitResult HitResult;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), true, HitResult)
		&& IsValid(HitResult.GetActor()))
	{
		// CanSelect was called already, so should have a selection component
		return HitResult.GetActor()->GetComponentByClass<UCombatComponent>();
	}

	return nullptr;
}

FVector ALordPlayerController::GetWorldPositionUnderMouse() const
{
	FHitResult HitResult;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), true, HitResult))
	{
		// CanSelect was called already, so should have a selection component
		return HitResult.Location;
	}

	return {};
}

void ALordPlayerController::SetCursorMode(EMouseCursor::Type Mode)
{
	CurrentMouseCursor = Mode;
	
	// Have to refocus the viewport to update :(
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void ALordPlayerController::OnMouseClick(bool bRightButton)
{
	if (bRightButton)
	{
		if (PlacementComponent->IsPlacing())
		{
			PlacementComponent->CancelPlacing();
		}
		else if (SpellCastingComponent->IsCasting())
		{
			SpellCastingComponent->CancelCasting();
		}
		else
		{
			// Flag!
			ERewardFlagType FlagType = ERewardFlagType::Explore;
			FVector At;
			UCombatComponent* AttachComponent = nullptr;
			bool bCanPlace = true;
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
				auto AtDirect = GetWorldPositionUnderMouse();
				At = AtDirect + FVector(0, 0, 500);

				// Make sure we can actually place it
				{
					const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
					const ANavigationData* NavData = NavSys->GetNavDataForProps(GetNavAgentPropertiesRef(), GetNavAgentLocation());
					const FVector Start = GetTeamState()->GetCastle()->GetActorLocation();
					bCanPlace = NavSys->TestPathSync(FPathFindingQuery(this, *NavData, Start, AtDirect, nullptr), EPathFindingMode::Hierarchical);
				}
			}

			if (bCanPlace)
			{
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
	}
	else
	{
		if (PlacementComponent->IsPlacing())
		{

			if (auto PlaceOperation = PlacementComponent->AttemptToPlace())
			{
				// If not authoritative, make sure to clean up the locally-spawned building
				if (!HasAuthority() && PlaceOperation->PlacedBuilding)
				{
					PlaceOperation->PlacedBuilding->Destroy();
					PlaceOperation->PlacedBuilding = nullptr;
				}

				Server_PlaceBuilding(MoveTemp(PlaceOperation.GetValue()));
				PlacementComponent->CancelPlacing();
			}
		}
		else if (SpellCastingComponent->IsCasting())
		{
			if (auto CastOperation = SpellCastingComponent->AttemptToCast())
			{
				Server_CastSpell(MoveTemp(CastOperation.GetValue()));

				// Do not deselect spell, so it can be cast over and over
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

	if (!TeamStateReady.IsComplete())
	{
		// Checking GetLordPlayerState() first to avoid an ensure
		if (GetLordPlayerState() && GetTeamState())
		{
			TeamStateReady.Complete(true);
		}
	}

	// Update hover state
	if (IsLocalPlayerController())
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

void ALordPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	auto State = GetLordPlayerState();
	check(State);
	PlayerStateReady.Complete(State);
	PlayerStateReadyVoid.Complete(true);
}

void ALordPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	auto State = GetLordPlayerState();
	if (State && !PlayerStateReady.IsComplete())
	{
		PlayerStateReady.Complete(State);
		PlayerStateReadyVoid.Complete(true);
	}
}

ULordGameHUDWidget* ALordPlayerController::ConstructHUD_Implementation()
{
	if (ensure(IsValid(HUDClass)))
	{
		return CreateWidget<ULordGameHUDWidget>(this, HUDClass);
	}

	return nullptr;
}

bool ALordPlayerController::CanSelect(const AActor* ClickedActor) const
{
	return IsValid(ClickedActor->GetComponentByClass<USelectionComponent>());
}

void ALordPlayerController::OnSelectionChange()
{
	// Update VM
	SelectionVM->SetFromSelection(HasSelection() ? GetSelection() : nullptr, true, true);
	BP_OnSelectionChange();
}

void ALordPlayerController::OnHoverChange()
{
	BP_OnHoverChange();
}

void ALordPlayerController::Server_PlaceBuilding_Implementation(FPlaceBuildingOperation PlaceOperation)
{
	if (!ensure(PlaceOperation.BuildingType))
	{
		return;
	}

	if (!ensure(PlaceOperation.PurchaseAction))
	{
		return;
	}

	auto State = GetTeamState();
	if (!ensure(State))
	{
		return;
	}

	// If called from an already-authoritative process, PlaceOperation is the direct output from
	// a placement component and the data is accurate -- including the still-alive building.
	// Otherwise, we have to replay a bit.
	// PlacedBuilding is nulled out and not replicated from client->server,
	// so this indicates if this came across the wire or not
	if (PlaceOperation.PlacedBuilding)
	{
		// Trust our own price.
		// Can't recalc either, since now there is +1 of those buildings
		//PlaceOperation.PlaceCost = ...;


	}
	else
	{
		// Get the price, and compare to what we received
		const int ServerPrice = PlaceOperation.PurchaseAction->CalculatePriceDirect(State);
		if (!ServerPrice == PlaceOperation.PlaceCost)
		{
			UE_LOG(LordBuilding, Error, TEXT("Client submitted price for building does not match server calculation (Client: %d,  Server: %d)"),
				PlaceOperation.PlaceCost, ServerPrice);

			PlaceOperation.PlaceCost = ServerPrice;
		}
	}

	// Now that we have cost, make sure we can afford it
	if (State->GetGold() < PlaceOperation.PlaceCost)
	{
		UE_LOG(LordBuilding, Warning, TEXT("Could not afford building placement late in the placement process"));
		return;
	}

	// Now do the actual placement

	if (!PlaceOperation.PlacedBuilding)
	{
		// This is a dupe of what happens in PlacementComponent. Maybe better to make a func to expose this for one codepath?
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform Transform(PlaceOperation.At);
		PlaceOperation.PlacedBuilding = GetWorld()->SpawnActor<ABuilding>(PlaceOperation.BuildingType->BuildingClass, Transform, Params);
		if (!ensure(PlaceOperation.PlacedBuilding))
		{
			return;
		}
	}

	// Commit

	State->AddGold(-PlaceOperation.PlaceCost);
	PlaceOperation.PlacedBuilding->Server_SetTeam(GetTeam());
	PlaceOperation.PlacedBuilding->HandleBuildingPlacement();
}

void ALordPlayerController::Server_PlaceFlag_Implementation(FVector At)
{

}

void ALordPlayerController::Server_CastSpell_Implementation(FPlayerSpellCastOperation CastOperation)
{
	// Cost is part of the spell, so no need to get and then independently verify the cost here. Attempting to
	// cast will do that automatically!

	if (!ensure(CastOperation.Spell))
	{
		return;
	}

	if (!CastOperation.Spell->AttemptCast(this, CastOperation.TargetLocation, CastOperation.TargetComponent))
	{
		UE_LOG(LordCombat, Warning, TEXT("Got spell cast request but could not activate it"));
		return;
	}
}
