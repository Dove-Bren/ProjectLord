// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "SelectionComponent.generated.h"

class USelectionAction;

DECLARE_MULTICAST_DELEGATE(FOnSelected);
DECLARE_MULTICAST_DELEGATE(FOnDeselected);

// Represents a current selection, including what actions are available from it.
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
    TArray<USelectionAction*> AvailableActions;

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

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection")
    TArray<TSubclassOf<USelectionAction>> Actions;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Selection")
    TArray<USelectionAction*> ActionInstances;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Selection")
    bool bSelectable;

};
