// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingTypes.generated.h"

class UTexture2D;
class UStaticMesh;
class ABuilding;

UCLASS(BlueprintType)
class PROJECTLORD_API UBuildingType : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	FText BuildingName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	FText BuildingDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	TObjectPtr<UTexture2D> BuildingIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	TObjectPtr<UStaticMesh> BuildingPlaceShape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	TSubclassOf<ABuilding> BuildingClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	int BuildCost;

};
