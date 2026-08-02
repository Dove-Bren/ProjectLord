// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnitTypeCatalog.generated.h"

class UUnitType;

UCLASS(BlueprintType)
class PROJECTLORD_API UUnitTypeCatalog : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION(BlueprintPure, Category = "Unit Type")
	TArray<UUnitType*> GetUnitTypes() const { check(!bLoadingUnitTypes); return UnitTypes; }

private:
	TArray<UUnitType*> UnitTypes;

	bool bLoadingUnitTypes;
};
