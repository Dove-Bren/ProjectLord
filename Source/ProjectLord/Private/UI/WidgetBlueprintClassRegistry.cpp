// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/WidgetBlueprintClassRegistry.h"

#include "Engine/AssetManager.h"

UWidgetBlueprintClassRegistry::UWidgetBlueprintClassRegistry(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Large text for where settings should be grouped on the left
    CategoryName = "Project";

    // Smaller sub-heading for grouping
    SectionName = "Widget Blueprint Classes";
}

const UWidgetBlueprintClassRegistry* UWidgetBlueprintClassRegistry::Get()
{
    return GetDefault<UWidgetBlueprintClassRegistry>();
}
