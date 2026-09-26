// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Gameplay/Units/HeroEquipment.h"

#include "InventoryBlueprintLibrary.generated.h"

UCLASS()
class PROJECTLORD_API UInventoryBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "ItemStack", meta = (WorldContext = "WorldContextObject"))
    static UHeroItemStack* MakeItemStack(UObject* WorldContextObject, const UHeroItemDef* Def, int Count = 1);
};
