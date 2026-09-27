// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/AI/CombatControllerBase.h"

#include "BuildingController.generated.h"

class UBehaviorTree;
class UCombatComponent;

UCLASS(Blueprintable)
class PROJECTLORD_API ABuildingController : public ACombatControllerBase
{
    GENERATED_BODY()

public:
    ABuildingController();

    virtual void OnPossess(APawn* InPawn) override;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure)
    UBehaviorTree* GetBehaviorTree() const;
};
