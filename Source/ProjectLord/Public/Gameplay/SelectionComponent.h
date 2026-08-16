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
DECLARE_MULTICAST_DELEGATE(FOnRemoved);

UCLASS(BlueprintType)
class PROJECTLORD_API USelectionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USelectionComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;

    FOnSelected OnSelected;
    FOnDeselected OnDeselected;
    FOnRemoved OnRemoved;

    UFUNCTION(BlueprintPure)
    bool CanSelect() const { return bSelectable; }

    UFUNCTION(BlueprintCallable)
    void SetSelectable(bool bInSelectable) { bSelectable = bInSelectable; }

    UFUNCTION(BlueprintCallable)
    void Select();

    UFUNCTION(BlueprintCallable)
    void Deselect();

    // Runtime getters & setters
    UFUNCTION(BlueprintPure)
    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { Icon = InIcon; }

    UFUNCTION(BlueprintPure)
    FText GetName() const { return Name; }
    void SetName(FText InName) { Name = InName; }

    UFUNCTION(BlueprintPure)
    FText GetDescription() const { return Desc; }
    void SetDesc(FText InDesc) { Desc = InDesc; }

    UFUNCTION(BlueprintPure)
    FText GetCustomName() const { return CustomName; }
    void SetCustomName(FText InCustomName) { CustomName = InCustomName; }

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const { return Team; }
    void SetTeam(EGameTeam InTeam) { Team = InTeam; }

    UFUNCTION(BlueprintPure)
    UVMAction* GetActionVM() const { return ActionVM; }
    void SetActionVM(UVMAction* InVM) { ActionVM = InVM; }

    UFUNCTION(BlueprintPure)
    UVMCombatData* GetCombatDataVM() const { return CombatDataVM; }
    void SetCombatDataVM(UVMCombatData* InVM) { CombatDataVM = InVM; }

    UFUNCTION(BlueprintPure)
    UVMGold* GetGoldVM() const { return GoldVM; }
    void SetGoldVM(UVMGold* InVM) { GoldVM = InVM; }

    UFUNCTION(BlueprintPure)
    UVMLevel* GetLevelVM() const { return LevelVM; }
    void SetLevelVM(UVMLevel* InVM) { LevelVM = InVM; }

    UFUNCTION(BlueprintPure)
    UVMProgressQueue* GetQueueVM() const { return QueueVM; }
    void SetQueueVM(UVMProgressQueue* InVM) { QueueVM = InVM; }

    UFUNCTION(BlueprintPure)
    UVMSummarySlots* GetSlotsVM() const { return SlotsVM; }
    void SetSlotsVM(UVMSummarySlots* InVM) { SlotsVM = InVM; }

    UFUNCTION(BlueprintPure)
    const TArray<USelectionAction*>& GetAvailableActions() const { return ActionInstances; }

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
    FText Desc;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    FText CustomName;

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
