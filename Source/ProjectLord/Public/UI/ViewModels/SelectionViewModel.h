// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "Gameplay/GameTeam.h"

#include "SelectionViewModel.generated.h"

class UVMAction;
class UVMCombatData;
class UVMGold;
class UVMLevel;
class UVMProgressQueue;
class UVMSummarySlots;
class USelectionComponent;
class UTexture2D;
class UVMSelectionAction;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMSelection : public UVMLordBase
{
    GENERATED_BODY()

public:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    FText SelectionName;
    void SetSelectionName(FText Name) { UE_MVVM_SET_PROPERTY_VALUE(SelectionName, Name); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    EGameTeam Team;
    void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    TObjectPtr<UTexture2D> Icon;
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMGold> GoldVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMAction> ActionVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMCombatData> CombatDataVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMLevel> LevelVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMProgressQueue> ProgressQueueVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMSummarySlots> SlotsVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TArray<UVMSelectionAction*> Actions;

    bool GetOnSelectionChange() const { return OnSelectionChange; }

    void Reset(bool bTriggerUpdate = true);
    void TriggerSelectionChange() { UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(OnSelectionChange); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Trigger")
    bool OnSelectionChange;

    UPROPERTY(BlueprintReadOnly, Category = "Selection")
    TObjectPtr<USelectionComponent> SelectionComponent;

};
