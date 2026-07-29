// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AIController.h"

#include "BuildingController.generated.h"

class UBehaviorTree;
class UCombatComponent;

UCLASS(Blueprintable)
class PROJECTLORD_API ABuildingController : public AAIController
{
    GENERATED_BODY()

public:
    ABuildingController();

    virtual void OnPossess(APawn* InPawn) override;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure)
    UBehaviorTree* GetBehaviorTree() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure)
    UCombatComponent* GetTargetComponent() const;
};
