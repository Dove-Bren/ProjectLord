// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UnitTypes.generated.h"

class UTexture2D;
class AUnit;

UCLASS(BlueprintType)
class PROJECTLORD_API UUnitType : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit Type")
	FText UnitName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit Type")
	FText UnitDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit Type")
	TObjectPtr<UTexture2D> UnitIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit Type")
	TSubclassOf<AUnit> UnitClass;
};
