// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"

#include "LordPlayerController.generated.h"

class UInputMappingContext;

UCLASS(Blueprintable)
class PROJECTLORD_API ALordPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ALordPlayerController();

    virtual void BeginPlay() override;

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defaults")
    TObjectPtr<UInputMappingContext> StartingInputContext;
};
