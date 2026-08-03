// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "Gameplay/SelectionComponent.h" // For FSelectionData in optional
#include "Gameplay/SelectionAction.h"
#include "Gameplay/GameTeam.h"

#include "LordPlayerController.generated.h"

class UInputMappingContext;
class ALordPlayerState;
class UVMSelection;

UCLASS(Blueprintable)
class PROJECTLORD_API ALordPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ALordPlayerController();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure)
    ALordPlayerState* GetLordPlayerState() const;

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const;

    UFUNCTION(BlueprintPure, Category = "Selection")
    bool HasSelection() const { return Selection.IsSet(); };

    UFUNCTION(BlueprintPure, Category = "Selection")
    FSelectionData GetSelection() const { if (ensure(HasSelection())) return Selection.GetValue(); return {}; };

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void SetSelection(USelectionComponent* Selection);

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void ClearSelection(bool bBroadcast = true);

    UFUNCTION(BlueprintImplementableEvent, Category = "Selection", meta = (DisplayName="OnSelectionChange"))
    void BP_OnSelectionChange();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    FSelectionActionContext MakeSelectionContext();

protected:

    UFUNCTION(BlueprintCallable)
    void OnMouseClick(bool bRightButton);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defaults")
    TObjectPtr<UInputMappingContext> StartingInputContext;

    TOptional<FSelectionData> Selection;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMSelection> SelectionVM;

    bool CanSelect(const AActor* ClickedActor) const;

    void OnSelectionChange();
};
