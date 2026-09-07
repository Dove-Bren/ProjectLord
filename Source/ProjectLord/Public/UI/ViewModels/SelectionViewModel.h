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
class UVMSelectionActionTree;
class UVMCreatureCategory;
class UVMAppealMetrics;

struct FSelectionActionContext;

enum class ESelectionActionFailureReason : uint8;

USTRUCT(BlueprintType)
struct PROJECTLORD_API FStaticSelection
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESelectionActionFailureReason Failure;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UVMSelection : public UVMLordBase
{
    GENERATED_BODY()

public:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    FText SelectionName;
    void SetSelectionName(FText Name) { UE_MVVM_SET_PROPERTY_VALUE(SelectionName, Name); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    FText SelectionDesc;
    void SetSelectionDescription(FText Desc) { UE_MVVM_SET_PROPERTY_VALUE(SelectionDesc, Desc); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    FText CustomName;
    void SetCustomName(FText InCustomName) { UE_MVVM_SET_PROPERTY_VALUE(CustomName, InCustomName); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    EGameTeam Team;
    void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    TObjectPtr<UTexture2D> Icon;
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    ESelectionActionFailureReason FailureReason;
    void SetFailureReason(ESelectionActionFailureReason InFailureReason) { UE_MVVM_SET_PROPERTY_VALUE(FailureReason, InFailureReason); }

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
    TObjectPtr<UVMSelectionActionTree> ActionTreeVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMCreatureCategory> CategoryVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMAppealMetrics> AppealVM;

    // Represent whether there's data in this VM or not.
    // Set to false when the VM is cleared, including between selections
    UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "Selection")
    bool bPresent;
    void SetPresent(bool bInPresent) { UE_MVVM_SET_PROPERTY_VALUE(bPresent, bInPresent); }

    bool GetOnSelectionChange() const { return OnSelectionChange; }

    void Reset(bool bTriggerUpdate = true);
    void SetFromSelection(const USelectionComponent* Selection, bool bIncludeActions = true, bool bTriggerUpdate = true);
    void SetFromStaticElement(FStaticSelection StaticElement, bool bTriggerUpdate = true);
    void TriggerSelectionChange() { UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(OnSelectionChange); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Trigger")
    bool OnSelectionChange;

    UPROPERTY(BlueprintReadOnly, Category = "Selection")
    TObjectPtr<USelectionComponent> SelectionComponent;

};
