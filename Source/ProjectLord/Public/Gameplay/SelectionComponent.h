// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gameplay/GameTeam.h"

#include "SelectionComponent.generated.h"

class USelectionAction;
class UVMSelection;
class UVMAction;
class UVMCombatData;
class UVMGold;
class UVMLevel;
class UVMProgressQueue;
class UVMSummarySlots;
class UTexture2D;

DECLARE_MULTICAST_DELEGATE(FOnSelected);
DECLARE_MULTICAST_DELEGATE(FOnDeselected);

// Represents a current selection, including what actions are available from it.
// TODO: Maybe worth just removing and using the SelectionComponent, since it keeps having to have the same data on it?
USTRUCT(BlueprintType)
struct PROJECTLORD_API FSelectionData
{
    GENERATED_BODY()

public:

    FSelectionData() : FSelectionData(nullptr) {}

    FSelectionData(USelectionComponent* Component) : SelectedComponent(Component) {};

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TWeakObjectPtr<USelectionComponent> SelectedComponent;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    FText Name;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    EGameTeam Team;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TArray<USelectionAction*> AvailableActions;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMAction> ActionVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMCombatData> CombatDataVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMGold> GoldVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMLevel> LevelVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMProgressQueue> QueueVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMSummarySlots> SlotsVM;

};

UCLASS(BlueprintType)
class PROJECTLORD_API USelectionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USelectionComponent();

    virtual void BeginPlay() override;

    FOnSelected OnSelected;
    FOnDeselected OnDeselected;

    UFUNCTION(BlueprintPure)
    bool CanSelect() const { return bSelectable; }

    UFUNCTION(BlueprintCallable)
    void SetSelectable(bool bInSelectable) { bSelectable = bInSelectable; }

    UFUNCTION(BlueprintCallable)
    FSelectionData Select();

    UFUNCTION(BlueprintCallable)
    void Deselect();

    // Runtime setters
    void SetIcon(UTexture2D* InIcon) { Icon = InIcon; }
    void SetName(FText InName) { Name = InName; }
    void SetTeam(EGameTeam InTeam) { Team = InTeam; }
    void SetActionVM(UVMAction* InVM) { ActionVM = InVM; }
    void SetCombatDataVM(UVMCombatData* InVM) { CombatDataVM = InVM; }
    void SetGoldVM(UVMGold* InVM) { GoldVM = InVM; }
    void SetLevelVM(UVMLevel* InVM) { LevelVM = InVM; }
    void SetQueueVM(UVMProgressQueue* InVM) { QueueVM = InVM; }
    void SetSlotsVM(UVMSummarySlots* InVM) { SlotsVM = InVM; }

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection")
    TArray<TSubclassOf<USelectionAction>> Actions;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Selection")
    TArray<USelectionAction*> ActionInstances;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Selection")
    bool bSelectable;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    FText Name;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    EGameTeam Team;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMAction> ActionVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMCombatData> CombatDataVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMGold> GoldVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMLevel> LevelVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMProgressQueue> QueueVM;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    TObjectPtr<UVMSummarySlots> SlotsVM;
};
