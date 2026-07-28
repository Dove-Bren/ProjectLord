// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/UnitTypeCatalog.h"
#include "Engine/AssetManager.h"

#include "Gameplay/Units/UnitTypes.h"

template<class T>
static void GetPrimaryAssetsOfType(const FPrimaryAssetType& InType, TFunction<void(TArray<T*>)> OnComplete)
{
	UAssetManager& Manager = UAssetManager::Get();

	Manager.LoadPrimaryAssetsWithType(InType, {}, FStreamableDelegate::CreateLambda([&InType, OnComplete]()
		{
			UAssetManager& Manager = UAssetManager::Get();
			TArray<FAssetData> Assets;
			TArray<T*> Results;

			Manager.GetPrimaryAssetDataList(InType, Assets);
			for (const FAssetData& AssetData : Assets)
			{
				T* CastAsset = Cast<T>(AssetData.GetAsset());
				if (CastAsset)
				{
					Results.Add(CastAsset);
				}
			}

			OnComplete(Results);
		}
	));
}


static FPrimaryAssetType CreatureTypeAssetType = FPrimaryAssetType(TEXT("CreatureType"));
static FPrimaryAssetType BuildingTypeAssetType = FPrimaryAssetType(TEXT("BuildingType"));

void UUnitTypeCatalog::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bLoadingCreatureTypes = true;
	GetPrimaryAssetsOfType<UCreatureType>(CreatureTypeAssetType, [this](TArray<UCreatureType*> Results) {
		CreatureTypes = Results;
		bLoadingCreatureTypes = false;
		UE_LOG(LogTemp, Log, TEXT("Loaded %d Creature Types"), CreatureTypes.Num());
	});

	GetPrimaryAssetsOfType<UBuildingType>(BuildingTypeAssetType, [this](TArray<UBuildingType*> Results) {
		BuildingTypes = Results;
		bLoadingBuildingTypes = false;
		UE_LOG(LogTemp, Log, TEXT("Loaded %d Building Types"), BuildingTypes.Num());
	});
}

void UUnitTypeCatalog::Deinitialize()
{
	Super::Deinitialize();

	ensure(!bLoadingCreatureTypes);
	ensure(!bLoadingBuildingTypes);

	UAssetManager& Manager = UAssetManager::Get();
	CreatureTypes.Empty();
	BuildingTypes.Empty();
	Manager.UnloadPrimaryAssetsWithType(CreatureTypeAssetType);
	Manager.UnloadPrimaryAssetsWithType(BuildingTypeAssetType);
}
