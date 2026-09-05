// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Map.h"

#include "Kismet/GameplayStatics.h"
#include "Landscape.h"

#include "LordLogging.h"

/*static*/ AMap* AMap::GetMap(const UObject* WorldContext)
{
	return Cast<AMap>(UGameplayStatics::GetActorOfClass(WorldContext, AMap::StaticClass()));
}

AMap::AMap()
{
	bReady = false;
}

void AMap::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoMapSize)
	{
		UE_LOG(LordMap, Log, TEXT("Trying to find auto map size"));

		// Deduce world size by looking for landscape
		auto Landscape = UGameplayStatics::GetActorOfClass(this, ALandscape::StaticClass());
		if (ensure(Landscape))
		{
			FVector Origin;
			FVector Extent;
			Landscape->GetActorBounds(false, Origin, Extent, true);
			MapMinPoint = Origin - FVector(Extent.X, Extent.Y, Extent.Z);
			MapWidth = Extent.X * 2;
			MapHeight = Extent.Y * 2;
			UE_LOG(LordMap, Log, TEXT("Read world size from landscape: %f x %f (origin: %f, %f, %f)"),
				MapWidth, MapHeight,
				MapMinPoint.X, MapMinPoint.Y, MapMinPoint.Z
			);
		}
	}

	if (!ensure(!FMath::IsNearlyZero(MapWidth) && !FMath::IsNearlyZero(MapHeight)))
	{
		UE_LOG(LordMap, Error, TEXT("World size is zero; inserting bogus values"));
		MapWidth = 1000;
		MapHeight = 1000;
	}

	bReady = true;
	OnMapReady.Broadcast();
}

void AMap::AddMapComponent(UMinimapComponent* Component)
{
	if (!MapComponents.Contains(Component))
	{
		MapComponents.AddUnique(Component);
		OnComponentsChange.Broadcast();
	}
}

void AMap::RemoveMapComponent(UMinimapComponent* Component)
{
	if (MapComponents.Remove(Component))
	{
		OnComponentsChange.Broadcast();
	}
}
