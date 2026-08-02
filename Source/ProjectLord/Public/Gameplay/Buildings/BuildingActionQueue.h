// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Gameplay/Buildings/QueuedAction.h"

#include "BuildingActionQueue.generated.h"

class UVMProgressQueue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionReady, UQueuedAction*, ReadyAction);
DECLARE_MULTICAST_DELEGATE(FOnQueueChange);

UCLASS(BlueprintType)
class PROJECTLORD_API UBuildingActionQueueComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBuildingActionQueueComponent();

    virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(BlueprintAssignable, Category = "Building|Queue")
    FOnActionReady OnActionReady;

    FOnQueueChange OnQueueChange;

    UFUNCTION(BlueprintPure, Category = "Building|Queue")
    TArray<UQueuedAction*> GetQueue() const { return Queue; }

    UFUNCTION(BlueprintPure, Category = "Building|Queue")
    bool HasActionInProgress() const;

    UFUNCTION(BlueprintPure, Category = "Building|Queue")
    bool IsFull() const;

    UFUNCTION(BlueprintCallable, Category = "Building|Queue")
    void QueueAction(UQueuedAction* InAction);

    UFUNCTION(BlueprintPure, Category = "Building|Queue")
    float GetTimeRemaining() const { return TimeRemaining; }

    // Returns a value from 0 to 1 indicating how close the building is to finishing
    // its current task
    UFUNCTION(BlueprintPure, Category = "Building|Queue")
    float GetProgress() const;

    UVMProgressQueue* GetViewModel() const { return ViewModel; }


protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Queue")
    TArray<UQueuedAction*> Queue;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Queue")
    float TimeRemaining;

    UPROPERTY()
    TObjectPtr<UVMProgressQueue> ViewModel;

    void ResetTimer();

    virtual float GetTotalTimeFor(const UQueuedAction* Action) const;

    UQueuedAction* GetActiveAction() const;

    void PerformAction(UQueuedAction* Action);

    void NotifyQueueChange();

};
