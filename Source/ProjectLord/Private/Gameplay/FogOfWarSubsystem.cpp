// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/FogOfWarSubsystem.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Misc/EnumRange.h"
#include "Landscape.h"
#include "Engine/PostProcessVolume.h"

#include "LordLogging.h"
#include "Gameplay/FogOfWar.h"
#include "Gameplay/FogOfWarComponent.h"

/*static*/ FName UFogOfWarSubsystem::BrushParam_Location = TEXT("Location");
/*static*/ FName UFogOfWarSubsystem::BrushParam_Radius = TEXT("Radius");
/*static*/ FName UFogOfWarSubsystem::BrushParam_Texture = TEXT("Texture");
/*static*/ FName UFogOfWarSubsystem::BrushParam_WorldMinOffset = TEXT("WorldMinOffset");
/*static*/ FName UFogOfWarSubsystem::BrushParam_WorldSize = TEXT("WorldSize");

UFogOfWarSubsystem::UFogOfWarSubsystem()
{
	bActive = false;
}

bool UFogOfWarSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true; // IsValid(UGameplayStatics::GetActorOfClass(Outer, AFogOfWar::StaticClass()));
}

void UFogOfWarSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LordFogOfWar, Display, TEXT("UFogOfWarSubsystem::Initialize"));

	Super::Initialize(Collection);
	bActive = false;
}

void UFogOfWarSubsystem::Deinitialize()
{
	UE_LOG(LordFogOfWar, Display, TEXT("UFogOfWarSubsystem::Deinitialize"));

	Super::Deinitialize();

}

TStatId UFogOfWarSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFogOfWarSubsystem, STATGROUP_Tickables);
}

bool UFogOfWarSubsystem::IsTickable() const
{
	// Prevent ticking for CDO / template objects
	return !IsTemplate() && bActive;
}

void UFogOfWarSubsystem::Tick(float DeltaSeconds)
{
	DoRevealPass();
}

void UFogOfWarSubsystem::Activate(AFogOfWar* FoWSettings)
{
	UE_LOG(LordFogOfWar, Log, TEXT("UFogOfWarSubsystem::Activate"));
	if (!ensure(!IsActive()))
	{
		UE_LOG(LordFogOfWar, Error, TEXT("UFogOfWarSubsystem::Activate: Already active!"));
	}

	if (!ensure(FoWSettings))
	{
		UE_LOG(LordFogOfWar, Error, TEXT("Failed to find Fog of War actor in world"));
		return;
	}

	UE_LOG(LordFogOfWar, Log, TEXT("Reading Fog Of War settings from FogOfWar actor"));
	FogSheetScale = FoWSettings->MapScale;
	FogRenderBrush = UMaterialInstanceDynamic::Create(FoWSettings->BrushMaterial, this);
	bActive = true;

	if (FoWSettings->bAutoMapSize)
	{
		// Deduce world size by looking for landscape
		auto Landscape = UGameplayStatics::GetActorOfClass(this, ALandscape::StaticClass());
		if (ensure(Landscape))
		{
			FVector Origin;
			FVector Extent;
			Landscape->GetActorBounds(false, Origin, Extent, true);
			WorldMin = Origin - FVector(Extent.X, Extent.Y, Extent.Z);
			WorldSize = FVector(Extent.X * 2, Extent.Y * 2, Extent.Z * 2);
			UE_LOG(LordFogOfWar, Log, TEXT("Read world size from landscape: %f x %f (origin: %f, %f, %f)"),
				WorldSize.X, WorldSize.Y,
				WorldMin.X, WorldMin.Y, WorldMin.Z
			);
		}
	}
	else
	{
		WorldSize = FVector(FoWSettings->MapWidth, FoWSettings->MapHeight, 0);
		WorldMin = FoWSettings->MapMinPoint;
	}

	if (!ensure(!WorldSize.IsNearlyZero()))
	{
		UE_LOG(LordFogOfWar, Error, TEXT("World size is zero; inserting bogus values"));
		WorldSize = FVector(128, 128, 100);
	}

	// Use world size to decide map size, and create render targets
	FogSheetWidth = FMath::CeilToInt(WorldSize.X / (float) FogSheetScale);
	FogSheetHeight = FMath::CeilToInt(WorldSize.Y / (float) FogSheetScale);
	UE_LOG(LordFogOfWar, Log, TEXT("Calculated FoW Sheet size: %d x %d"), FogSheetWidth, FogSheetHeight);
	for (EGameTeam Team : TEnumRange<EGameTeam>())
	{
		if (TeamHasFog(Team))
		{
			auto Target = UKismetRenderingLibrary::CreateRenderTarget2D(this, FogSheetWidth, FogSheetHeight, RTF_R8);
			UKismetRenderingLibrary::ClearRenderTarget2D(this, Target, FLinearColor::Transparent);
			FogRenderTargets.Add(Team, Target);

			auto GameplayMap = MakeUnique<bool[]>(FogSheetWidth * FogSheetHeight);
			GameplayMaps.Add(Team, MoveTemp(GameplayMap));
		}
	}

	// Set up world rendering
	WorldFogVolumeBrush = UMaterialInstanceDynamic::Create(FoWSettings->WorldFogMaterial, this);
	WorldFogVolumeBrush->SetVectorParameterValue(BrushParam_WorldMinOffset, WorldMin);
	WorldFogVolumeBrush->SetVectorParameterValue(BrushParam_WorldSize, WorldSize);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	WorldFogVolume = GetWorld()->SpawnActor<APostProcessVolume>(Params);
	WorldFogVolume->bUnbound = true;
	WorldFogVolume->bEnabled = true;
	WorldFogVolume->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, WorldFogVolumeBrush));

	SetWorldFogForTeam(FoWSettings->DefaultTeamToDisplay);

	for (EGameTeam Team : TEnumRange<EGameTeam>())
	{
		WorkMap.Emplace(Team);
	}
}

bool UFogOfWarSubsystem::IsInFog(EGameTeam Team, FVector Location) const
{
	if (!IsActive())
	{
		return false;
	}

	if (TeamHasFog(Team))
	{
		const int SheetX = FMath::FloorToInt(WorldXToSheetX(Location.X));
		const int SheetY = FMath::FloorToInt(WorldYToSheetY(Location.Y));
		if (SheetX < 0 || SheetX >= FogSheetWidth || SheetY < 0 || SheetY >= FogSheetHeight)
		{
			return true;
		}

		const int CellIdx = (SheetY * FogSheetWidth) + SheetX;
		return !GameplayMaps[Team][CellIdx];
	}

	return false;
}

void UFogOfWarSubsystem::AddRevealer(UFogOfWarComponent* Revealer)
{
	UE_LOG(LordFogOfWar, Verbose, TEXT("Registered revealer for %s"), *Revealer->GetOwner()->GetName());
	Components.AddUnique(Revealer);
}

void UFogOfWarSubsystem::RemoveRevealer(UFogOfWarComponent* Revealer)
{
	UE_LOG(LordFogOfWar, Verbose, TEXT("Remove revealer for %s"), *Revealer->GetOwner()->GetName());
	Components.Remove(Revealer);
}

void UFogOfWarSubsystem::RevealFog(EGameTeam Team, FVector Location, float Radius)
{
	if (!IsActive())
	{
		return;
	}

	UE_LOG(LordFogOfWar, Verbose, TEXT("Manually revealing spot"));
	ExtraRevealSpots.Emplace(Team, Location, Radius);
}

UTextureRenderTarget2D* UFogOfWarSubsystem::GetTeamFogTarget(EGameTeam Team) const
{
	if (!IsActive() || !TeamHasFog(Team))
	{
		return nullptr;
	}

	return FogRenderTargets[Team];
}

void UFogOfWarSubsystem::SetWorldFogForTeam(EGameTeam Team)
{
	WorldFogVolumeBrush->SetTextureParameterValue(BrushParam_Texture, GetTeamFogTarget(Team));
}

void UFogOfWarSubsystem::DoRevealPass()
{
	if (!IsActive())
	{
		return;
	}

	for (auto Revealer : Components)
	{
		auto Team = Revealer->GetTeam();
		WorkMap[Team].Emplace(Revealer->GetRevealLocation(), Revealer->GetRevealRadius());
	}

	// Add the things from the extra spots
	for (auto Extra : ExtraRevealSpots)
	{
		if (Extra.Get<2>() <= 0)
		{
			continue;
		}

		WorkMap[Extra.Get<0>()]
			.Emplace(Extra.Get<1>(), Extra.Get<2>());
	}

	ExtraRevealSpots.Empty();

	// Reveal all these areas. VFX? DrawMaterial call?
	for (EGameTeam Team : TEnumRange<EGameTeam>())
	{
		if (!TeamHasFog(Team))
		{
			continue;
		}

		for (auto Point : WorkMap[Team])
		{
			float SheetX = WorldXToSheetX(Point.Key.X);
			float SheetY = WorldYToSheetY(Point.Key.Y);
			float Radius = FMath::Clamp((Point.Value / (double)WorldSize.X), 0, .25);

			// Update high-quality GPU render target
			// TODO batch this up somehow
			{
				float U = SheetXToSheetU(SheetX);
				float V = SheetYToSheetV(SheetY);
				FogRenderBrush->SetVectorParameterValue(BrushParam_Location, FVector(U, V, 0));
				FogRenderBrush->SetScalarParameterValue(BrushParam_Radius, Radius);
				UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), FogRenderTargets[Team], FogRenderBrush);
			}

			// Update low-quality gameplay driving map
			{
				const int SheetXCenter = FMath::FloorToInt(SheetX);
				const int SheetYCenter = FMath::FloorToInt(SheetY);
				const int RadiusCells = FMath::FloorToInt(Point.Value / FogSheetScale);
				const int RadiusCellsSqr = RadiusCells * RadiusCells;

				// Iterate over square containing circle we'll draw
				for (int Y = SheetYCenter - RadiusCells; Y <= SheetYCenter + RadiusCells; Y++)
				for (int X = SheetXCenter - RadiusCells; X <= SheetXCenter + RadiusCells; X++)
				{
					// Optimize this out by rolling into bounds
					if (Y < 0 || X < 0 || Y >= FogSheetHeight || X >= FogSheetWidth)
					{
						continue;
					}

					const int HDistSqr = FMath::Square(X - SheetXCenter) + FMath::Square(Y - SheetYCenter);
					if (HDistSqr <= RadiusCellsSqr)
					{
						const int CellIdx = (Y * FogSheetWidth) + X;
						GameplayMaps[Team][CellIdx] = true;
					}
				}
			}
		}
	}

	// Clear work map, but don't free space
	for (EGameTeam Team : TEnumRange<EGameTeam>())
	{
		WorkMap[Team].Empty();
	}
}
