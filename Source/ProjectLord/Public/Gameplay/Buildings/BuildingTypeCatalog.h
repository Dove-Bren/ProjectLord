// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuildingTypeCatalog.generated.h"

class UBuildingType;

UCLASS(BlueprintType)
class PROJECTLORD_API UBuildingTypeCatalog : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION(BlueprintPure, Category = "Building Type")
	TArray<UBuildingType*> GetBuildingTypes() const { check(!bLoadingBuildingTypes); return BuildingTypes; }

private:
	TArray<UBuildingType*> BuildingTypes;

	bool bLoadingBuildingTypes;
};
