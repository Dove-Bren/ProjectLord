// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/EquipmentCatalog.h"
#include "Engine/AssetManager.h"

#include "AssetHelper.h"
#include "LordLogging.h"
#include "Gameplay/Units/HeroEquipment.h"

static FPrimaryAssetType ItemDefAssetType = FPrimaryAssetType(TEXT("ItemDef"));

void UEquipmentCatalog::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bLoadingItemDefs = true;
	GetPrimaryAssetsOfType<UHeroItemDef>(ItemDefAssetType, [this](TArray<UHeroItemDef*> Results) {
		ItemDefs = Results;
		bLoadingItemDefs = false;
		UE_LOG(LordUnit, Log, TEXT("Loaded %d Item Defs"), ItemDefs.Num());
		});
}

void UEquipmentCatalog::Deinitialize()
{
	Super::Deinitialize();

	ensure(!bLoadingItemDefs);

	UAssetManager& Manager = UAssetManager::Get();
	ItemDefs.Empty();
	Manager.UnloadPrimaryAssetsWithType(ItemDefAssetType);
}
