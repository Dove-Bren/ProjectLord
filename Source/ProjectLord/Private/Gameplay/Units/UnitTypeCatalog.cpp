// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/UnitTypeCatalog.h"
#include "Engine/AssetManager.h"

#include "AssetHelper.h"
#include "LordLogging.h"
#include "Gameplay/Units/UnitTypes.h"

static FPrimaryAssetType UnitTypeAssetType = FPrimaryAssetType(TEXT("UnitType"));

void UUnitTypeCatalog::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bLoadingUnitTypes = true;
	GetPrimaryAssetsOfType<UUnitType>(UnitTypeAssetType, [this](TArray<UUnitType*> Results) {
		UnitTypes = Results;
		bLoadingUnitTypes = false;
		UE_LOG(LordUnit, Log, TEXT("Loaded %d Unit Types"), UnitTypes.Num());
	});
}

void UUnitTypeCatalog::Deinitialize()
{
	Super::Deinitialize();

	ensure(!bLoadingUnitTypes);

	UAssetManager& Manager = UAssetManager::Get();
	UnitTypes.Empty();
	Manager.UnloadPrimaryAssetsWithType(UnitTypeAssetType);
}
