// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LandscapePainter.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Landscape.h"

#include "Gameplay/Map.h"

/*static*/ FName ALandscapePainter::BrushParam_Location = TEXT("Location");
/*static*/ FName ALandscapePainter::BrushParam_Radius = TEXT("Radius");

/*static*/ ALandscapePainter* ALandscapePainter::GetLandscapePainter(const UObject* WorldContext)
{
	return Cast<ALandscapePainter>(UGameplayStatics::GetActorOfClass(WorldContext, ALandscapePainter::StaticClass()));
}

ALandscapePainter::ALandscapePainter()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ALandscapePainter::BeginPlay()
{
	Super::BeginPlay();

	bReady = false;
	BrushInstance = UMaterialInstanceDynamic::Create(BrushMaterial, this);
}

void ALandscapePainter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bReady)
	{
		auto Map = AMap::GetMap(this);
		if (Map && Map->IsReady())
		{
			SetupLayers();
			bReady = true;
		}

	}
	// else why ticking?
}

void ALandscapePainter::Paint(ELandscapePaint Type, FVector Location, float Radius)
{
	ensure(bReady);

	auto LayerTex = DynamicLandscapeLayers[Type];
	check(LayerTex);

	const float SheetU = (float) ((Location.X - (double) WorldOffsetX) / (double)WorldWidth);
	const float SheetV = (float)((Location.Y - (double)WorldOffsetY) / (double)WorldHeight);
	Radius = FMath::Clamp((Radius / (double)WorldWidth), 0, .25);

	{
		BrushInstance->SetVectorParameterValue(BrushParam_Location, FVector(SheetU, SheetV, 0));
		BrushInstance->SetScalarParameterValue(BrushParam_Radius, Radius);
	}

	// Expensive but that's okay, probably
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), LayerTex, BrushInstance);
}

void ALandscapePainter::SetupLayers()
{
	auto Map = AMap::GetMap(this);
	if (ensure(Map && Map->IsReady()))
	{
		WorldWidth = Map->GetMapWidth();
		WorldHeight = Map->GetMapHeight();
		auto Min = Map->GetMapMinPoint();
		WorldOffsetX = Min.X;
		WorldOffsetY = Min.Y;
	}

	for (auto Layer : TEnumRange<ELandscapePaint>())
	{
		auto LayerTex = UKismetRenderingLibrary::CreateRenderTarget2D(this, FMath::FloorToInt(WorldWidth / LayerScale), FMath::FloorToInt(WorldHeight / LayerScale), RTF_R8);
		DynamicLandscapeLayers.Add(Layer, LayerTex);
	}

	// Inject into landscape material
	// Deduce world size by looking for landscape
	auto Landscape = Cast<ALandscape>(UGameplayStatics::GetActorOfClass(this, ALandscape::StaticClass()));
	if (ensure(Landscape))
	{
		for (auto LandscapeComp : Landscape->LandscapeComponents)
		{
			if (!ensure(LandscapeComp->GetLandscapeProxy()->bUseDynamicMaterialInstance))
			{
				continue;
			}

			for (auto MatInst : LandscapeComp->MaterialInstancesDynamic)
			{
				for (auto Layer : TEnumRange<ELandscapePaint>())
				{
					//DynLayer_Foundation
					FName LayerParam = *FString::Printf(TEXT("DynLayer_%s"), *UEnum::GetDisplayValueAsText(Layer).ToString());
					MatInst->SetTextureParameterValue(LayerParam, DynamicLandscapeLayers[Layer]);
				}
			}
		}
	}
}
