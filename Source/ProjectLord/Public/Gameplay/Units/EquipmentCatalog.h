// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentCatalog.generated.h"

class UHeroItemDef;

UCLASS(BlueprintType)
class PROJECTLORD_API UEquipmentCatalog : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION(BlueprintPure, Category = "Item Defs")
	TArray<UHeroItemDef*> GetItemDefs() const { check(!bLoadingItemDefs); return ItemDefs; }

private:
	TArray<UHeroItemDef*> ItemDefs;

	bool bLoadingItemDefs;
};
