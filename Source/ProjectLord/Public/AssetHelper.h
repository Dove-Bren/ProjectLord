// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
//#include "AssetHelper.generated.h"

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