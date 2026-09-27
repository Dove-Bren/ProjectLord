// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CombatControllerBase.generated.h"

class UCombatComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAITargetChange, UCombatComponent*);

UCLASS(Blueprintable)
class PROJECTLORD_API ACombatControllerBase : public AAIController
{
    GENERATED_BODY()

public:
    
    virtual void OnPossess(APawn* InPawn) override;

    FOnAITargetChange OnAITargetChange;

    UFUNCTION(BlueprintPure)
    UCombatComponent* GetTargetComponent() const { return Target; }

    void SetTarget(UCombatComponent* InTarget) { if (Target != InTarget) { Target = InTarget; OnAITargetChange.Broadcast(Target); } }

    UFUNCTION(BlueprintCallable)
    void OverrideTarget(UCombatComponent* InTarget);

    void NotifyPawnDied();
    void NotifyPawnAttacked(AActor* AttackingActor, UCombatComponent* AttackingCombatComponent);

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UCombatComponent> Target;

    virtual void OnBehaviorTreeStarted();

private:
    EBlackboardNotificationResult OnBBTargetChanged(const UBlackboardComponent&, FBlackboard::FKey keyID);

    UFUNCTION()
    void OnPawnAttacked(AActor* AttackingActor, UCombatComponent* AttackingCombatComponent);
};
