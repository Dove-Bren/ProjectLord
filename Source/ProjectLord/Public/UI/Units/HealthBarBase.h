// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "Gameplay/GameTeam.h"

#include "HealthBarBase.generated.h"

class UVMCombatData;

UCLASS()
class PROJECTLORD_API UHealthBarBase : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void ReceiveCombatVM(UVMCombatData* InViewModel);

    UFUNCTION(BlueprintImplementableEvent)
    void ReceiveTeam(EGameTeam InTeam);
};
