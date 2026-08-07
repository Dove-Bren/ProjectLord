// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "UnitController.generated.h"

class UBehaviorTree;
class UCombatComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAITargetChange, UCombatComponent*);

UCLASS(Blueprintable)
class PROJECTLORD_API AUnitController : public AAIController
{
    GENERATED_BODY()

public:
    AUnitController();

    virtual void OnPossess(APawn* InPawn) override;

    FOnAITargetChange OnAITargetChange;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure)
    UBehaviorTree* GetBehaviorTree() const;

    UFUNCTION(BlueprintPure)
    UCombatComponent* GetTargetComponent() const { return Target; }

    UFUNCTION(BlueprintCallable)
    void SetTarget(UCombatComponent* InTarget) { if (Target != InTarget) { Target = InTarget; OnAITargetChange.Broadcast(Target); } }

    void NotifyUnitDied();

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UCombatComponent> Target;

private:
    EBlackboardNotificationResult OnBBTargetChanged(const UBlackboardComponent&, FBlackboard::FKey keyID);
};
