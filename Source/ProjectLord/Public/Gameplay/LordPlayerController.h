// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/SelectionAction.h"
#include "Gameplay/GameTeam.h"

#include "LordPlayerController.generated.h"

class UInputMappingContext;
class ALordPlayerState;
class UVMSelection;
class UPlacementComponent;

struct FStaticSelection;

UCLASS(Blueprintable)
class PROJECTLORD_API ALordPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ALordPlayerController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintPure)
    ALordPlayerState* GetLordPlayerState() const;

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const;

    UFUNCTION(BlueprintPure, Category = "Selection")
    bool HasSelection() const { return Selection.IsSet(); };

    UFUNCTION(BlueprintPure, Category = "Selection")
    USelectionComponent* GetSelection() const { if (ensure(HasSelection())) return Selection.GetValue(); return {}; };

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void SetSelection(USelectionComponent* Selection);

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void ClearSelection(bool bBroadcast = true);

    UFUNCTION(BlueprintImplementableEvent, Category = "Selection", meta = (DisplayName="OnSelectionChange"))
    void BP_OnSelectionChange();

    UFUNCTION(BlueprintPure, Category = "Hover")
    bool HasHover() const { return bHasHoverInfo; };

    /*UFUNCTION(BlueprintPure, Category = "Hover")
    USelectionComponent* GetHover() const { if (ensure(HasHover())) return Hovered.GetValue(); return {}; };*/

    UFUNCTION(BlueprintCallable, Category = "Hover")
    void SetHovered(USelectionComponent* InHovered);

    UFUNCTION(BlueprintCallable, Category = "Hover")
    void SetHoveredStaticElement(FStaticSelection StaticElement);

    UFUNCTION(BlueprintImplementableEvent, Category = "Hover", meta = (DisplayName = "OnHoverChange"))
    void BP_OnHoverChange();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    FSelectionActionContext MakeSelectionContext();

    UFUNCTION(BlueprintCallable, Category = "Placement")
    void PlaceBuilding(UBuildingType* Type, int Cost);

protected:

    UFUNCTION(BlueprintCallable)
    void OnMouseClick(bool bRightButton);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defaults")
    TObjectPtr<UInputMappingContext> StartingInputContext;

    TOptional<USelectionComponent*> Selection;
    TOptional<USelectionComponent*> HoveredComponent;

    bool bHasHoverInfo;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMSelection> SelectionVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMSelection> HoverVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UPlacementComponent> PlacementComponent;

    USelectionComponent* GetSelectableUnderMouse();
    bool CanSelect(const AActor* ClickedActor) const;

    void OnSelectionChange();
    void OnHoverChange();
};
