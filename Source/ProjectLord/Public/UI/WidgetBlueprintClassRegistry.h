// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprintClassRegistry.generated.h"

// A config that stores class references and other assets that are relevant to the UI.
// Can be used to reference these classes from code.
UCLASS(config = Game, DefaultConfig)
class PROJECTLORD_API UWidgetBlueprintClassRegistry : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UWidgetBlueprintClassRegistry(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetWidgetBlueprintClassRegistry"))
    static const UWidgetBlueprintClassRegistry* Get();

    // The small healthbar that appears over units in the world
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Units")
    TSubclassOf<UUserWidget> UnitMiniHealthBarWidget;
};
