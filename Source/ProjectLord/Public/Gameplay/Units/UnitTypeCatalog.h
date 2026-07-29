// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnitTypeCatalog.generated.h"

class UCreatureType;

UCLASS(BlueprintType)
class PROJECTLORD_API UUnitTypeCatalog : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION(BlueprintPure, Category = "Creature Type")
	TArray<UCreatureType*> GetCreatureTypes() const { check(!bLoadingCreatureTypes); return CreatureTypes; }

private:
	TArray<UCreatureType*> CreatureTypes;

	bool bLoadingCreatureTypes;
};
