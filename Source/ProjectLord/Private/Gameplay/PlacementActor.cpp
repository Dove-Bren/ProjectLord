// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/PlacementActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

APlacementActor::APlacementActor()
{
	Collision = CreateDefaultSubobject<UBoxComponent>("Collision");
	RootComponent = Collision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Collision->SetGenerateOverlapEvents(true);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void APlacementActor::SetMesh(UStaticMesh* InMesh)
{
	Mesh->SetStaticMesh(InMesh);

	// Update collision, too
	const FVector Margin = FVector(50, 50, 0);
	auto Bounds = Mesh->GetBounds().GetBox();
	Collision->SetBoxExtent(Bounds.GetExtent() + Margin, true);
}

bool APlacementActor::HasOverlap() const
{
	TArray<AActor*> Overlaps;
	Collision->GetOverlappingActors(Overlaps);
	return !Overlaps.IsEmpty();
}

void APlacementActor::SetLocation(FVector GroundLocation)
{
	SetActorLocation(GroundLocation); // Need to offset by mesh bounds
}
