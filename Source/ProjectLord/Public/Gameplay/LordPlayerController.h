// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/SelectionAction.h"
#include "Gameplay/GameTeam.h"
#include "Gameplay/Units/RewardFlag.h"

#include "LordPlayerController.generated.h"

class UInputMappingContext;
class ALordPlayerState;
class UVMSelection;
class UPlacementComponent;
class UVMLordBase;
class UInspectWidget;
class ULordGameHUDWidget;

struct FStaticSelection;
struct FToastNotification;

UCLASS(Blueprintable)
class PROJECTLORD_API ALordPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ALordPlayerController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool SetPause(bool bPause, FCanUnpause CanUnpauseDelegate) override;

    UFUNCTION(BlueprintPure)
    ALordPlayerState* GetLordPlayerState() const;

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const;

    UFUNCTION(BlueprintPure)
    AGameTeamState* GetTeamState() const;

    UFUNCTION(BlueprintPure)
    ULordGameHUDWidget* GetHUDWidget() const { return HUDWidget; }

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

    UFUNCTION(BlueprintCallable, Category = "Hover")
    void ClearHovered(bool bBroadcast = true);

    UFUNCTION(BlueprintImplementableEvent, Category = "Hover", meta = (DisplayName = "OnHoverChange"))
    void BP_OnHoverChange();

    UFUNCTION(BlueprintCallable, Category = "Placement")
    void PlaceBuilding(UBuildingType* Type, int Cost);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inspection")
    bool ShowInspectWidget(TSubclassOf<UInspectWidget> WidgetClass, UVMLordBase* VM);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Focus")
    void SetFocusedActor(const AActor* Actor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Focus")
    void PanTo(FVector WorldPosition);

    UFUNCTION(BlueprintCallable, Category = "Toast")
    void AddToastNotification(FToastNotification Notification);

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defaults")
    TMap<ERewardFlagType, TSubclassOf<ARewardFlag>> FlagClasses;

    UFUNCTION(BlueprintCallable)
    void OnMouseClick(bool bRightButton);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defaults")
    TObjectPtr<UInputMappingContext> StartingInputContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defaults")
    TSubclassOf<ULordGameHUDWidget> HUDClass;

    TOptional<USelectionComponent*> Selection;
    TOptional<USelectionComponent*> HoveredComponent;

    bool bHasHoverInfo;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMSelection> SelectionVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMSelection> HoverVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UPlacementComponent> PlacementComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<ULordGameHUDWidget> HUDWidget;

    virtual void OnSetPaused(bool bPaused);

    UFUNCTION(BlueprintNativeEvent, Category = "HUD")
    ULordGameHUDWidget* ConstructHUD();

    UFUNCTION(BlueprintImplementableEvent, Category = "Selection", meta = (DisplayName = "OnSetPaused"))
    void BP_OnSetPaused(bool bPaused);

    USelectionComponent* GetSelectableUnderMouse();
    FVector GetWorldPositionUnderMouse();
    bool CanSelect(const AActor* ClickedActor) const;

    void OnSelectionChange();
    void OnHoverChange();
};
