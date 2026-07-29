// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/UnitTypeCatalog.h"
#include "Engine/AssetManager.h"

#include "AssetHelper.h"
#include "LordLogging.h"
#include "Gameplay/Buildings/BuildingTypes.h"

static FPrimaryAssetType BuildingTypeAssetType = FPrimaryAssetType(TEXT("BuildingType"));

void UBuildingTypeCatalog::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GetPrimaryAssetsOfType<UBuildingType>(BuildingTypeAssetType, [this](TArray<UBuildingType*> Results) {
		BuildingTypes = Results;
		bLoadingBuildingTypes = false;
		UE_LOG(LordBuilding, Log, TEXT("Loaded %d Building Types"), BuildingTypes.Num());
	});
}

void UBuildingTypeCatalog::Deinitialize()
{
	Super::Deinitialize();

	ensure(!bLoadingBuildingTypes);

	UAssetManager& Manager = UAssetManager::Get();
	BuildingTypes.Empty();
	Manager.UnloadPrimaryAssetsWithType(BuildingTypeAssetType);
}
