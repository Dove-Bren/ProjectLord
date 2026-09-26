// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/InventoryBlueprintLibrary.h"

#include "Gameplay/Units/HeroEquipment.h"

/*static*/ UHeroItemStack* UInventoryBlueprintLibrary::MakeItemStack(UObject* WorldContextObject, const UHeroItemDef* Def, int Count)
{
    return UHeroItemStack::Make(WorldContextObject, Def, Count);
}