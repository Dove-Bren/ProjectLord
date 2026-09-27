// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/AI/CombatControllerBase.h"

#include "UnitController.generated.h"

class UBehaviorTree;
class UCombatComponent;

UCLASS(Blueprintable)
class PROJECTLORD_API AUnitController : public ACombatControllerBase
{
    GENERATED_BODY()

public:
    AUnitController();

    virtual void OnPossess(APawn* InPawn) override;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure)
    UBehaviorTree* GetBehaviorTree() const;

};
