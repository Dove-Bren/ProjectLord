// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/UnitTypeCatalog.h"
#include "Engine/AssetManager.h"

#include "AssetHelper.h"
#include "Gameplay/Units/UnitTypes.h"

static FPrimaryAssetType CreatureTypeAssetType = FPrimaryAssetType(TEXT("CreatureType"));

void UUnitTypeCatalog::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bLoadingCreatureTypes = true;
	GetPrimaryAssetsOfType<UCreatureType>(CreatureTypeAssetType, [this](TArray<UCreatureType*> Results) {
		CreatureTypes = Results;
		bLoadingCreatureTypes = false;
		UE_LOG(LordUnit, Log, TEXT("Loaded %d Creature Types"), CreatureTypes.Num());
	});
}

void UUnitTypeCatalog::Deinitialize()
{
	Super::Deinitialize();

	ensure(!bLoadingCreatureTypes);

	UAssetManager& Manager = UAssetManager::Get();
	CreatureTypes.Empty();
	Manager.UnloadPrimaryAssetsWithType(CreatureTypeAssetType);
}
