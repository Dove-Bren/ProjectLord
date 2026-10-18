// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Interaction/AutoPlaceSelectionAction.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"

#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Player/LordPlayerController.h"

UAutoPlaceSelectionAction::UAutoPlaceSelectionAction()
{
	bRunLocal = false;
}

bool UAutoPlaceSelectionAction::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	if (!Super::CanPerform_Implementation(InContext, ReasonOut))
	{
		return false;
	}

	auto Building = Cast<ABuilding>(InContext.Selection->GetOwner());
	if (!IsValid(Building))
	{
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	return true;
}

bool UAutoPlaceSelectionAction::Perform_Implementation(const FSelectionActionContext& InContext) const
{
	// Note: not using 'this' for world context, because 'this' is a data asset and has no world
	ALordPlayerController* Controller = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(InContext.Selection, 0));
	if (!Controller)
	{
		return false;
	}

	if (!ensure(Controller->HasAuthority()))
	{
		return false;
	}

	// Double check placement is still allowed -- that there's nothing
	// on top of our building
	auto Building =  Cast<ABuilding>(InContext.Selection->GetOwner());
	if (!IsValid(Building))
	{
		return false;
	}

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Building);
	if (Controller->GetPawn())
	{
		Params.AddIgnoredActor(Controller->GetPawn());
	}

	auto Extent = GetBuildingType()->BuildingPlaceShape->GetBounds().BoxExtent;
	FCollisionShape CollisionShape = FCollisionShape::MakeBox(Extent);

	// Also need to adjust position up to match the bases of the extents
	auto MyExtent = Building->GetComponentsBoundingBox().GetExtent();


	FVector At = Building->GetActorLocation() + Extent.Z + 0.05;

	TArray<FOverlapResult> OverlapsOut;
	auto World = Building->GetWorld();
	World->OverlapMultiByObjectType(OverlapsOut, At, FQuat(), ObjectParams, CollisionShape, Params);
	//if (World->OverlapAnyTestByObjectType(Building->GetActorLocation(), FQuat(), ObjectParams, CollisionShape, Params))
	if (!OverlapsOut.IsEmpty())
	{
		return false;
	}

	DeductGoldCost(InContext);
	Building->Destroy();
	Controller->PlaceBuildingImmediately(GetBuildingType(), FTransform(Building->GetActorLocation()));
	return true;
}
