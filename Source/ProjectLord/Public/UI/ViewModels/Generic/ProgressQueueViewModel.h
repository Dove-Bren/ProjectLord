// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/QueuedAction.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "ProgressQueueViewModel.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMQueuedAction : public UVMLordBase
{
    GENERATED_BODY()

public:
    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

    static UVMQueuedAction* Make(UObject* Outer, UQueuedAction* Action)
    {
        auto VM = CreateLordVM<UVMQueuedAction>(Outer);
        VM->SetIcon(Action->GetIcon());
        return VM;
    }

    bool Matches(const UQueuedAction* Action) const
    {
        // We only actally care about the icon, so check that
        return Action->GetIcon() == Icon;
    }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Queued Action")
    TObjectPtr<UTexture2D> Icon;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UVMProgressQueue : public UVMLordBase
{
    GENERATED_BODY()

public:

    float GetProgress() const { return Progress; }
    void SetProgress(float InProgress) { UE_MVVM_SET_PROPERTY_VALUE(Progress, InProgress); }

    bool GetShowQueueActions() const { return ShowQueueActions; }
    void SetShowQueueActions(bool InShowQueueActions) { UE_MVVM_SET_PROPERTY_VALUE(ShowQueueActions, InShowQueueActions); }

    const TArray<UVMQueuedAction*>& GetQueue() const { return Queue; }
    void SetQueue(TArray<UVMQueuedAction*> InQueue) { UE_MVVM_SET_PROPERTY_VALUE(Queue, InQueue); }
    void UpdateQueue(const TArray<UQueuedAction*>& InQueue);

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Queue")
    float Progress;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Queue")
    TArray<UVMQueuedAction*> Queue;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Queue")
    bool ShowQueueActions;
};
