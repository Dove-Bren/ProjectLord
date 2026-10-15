// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "Gameplay/Interaction/SelectionComponent.h"
#include "Gameplay/Interaction/SelectionAction.h"
#include "Gameplay/GameTeam.h"
#include "Gameplay/Units/RewardFlag.h"
#include "UEUtils/MulticastFuture.h"

#include "LordPlayerController.generated.h"

class UInputMappingContext;
class ALordPlayerState;
class UVMSelection;
class UPlacementComponent;
class UVMLordBase;
class UInspectWidget;
class ULordGameHUDWidget;
class UCombatComponent;
class UAbilitySystemComponent;
class UPlayerSpell;
class UPlayerSpellCastingComponent;
class ULordCursorWidget;
class AGameTeamState;

struct FStaticSelection;
struct FToastNotification;
struct FSelectionActionContext;
struct FPlaceBuildingOperation;
struct FPlayerSpellCastOperation;

USTRUCT()
struct PROJECTLORD_API FFlagPlaceOperation
{
    GENERATED_BODY()

public:

    UPROPERTY()
    ERewardFlagType Type = ERewardFlagType::Explore;

    UPROPERTY()
    FVector FlagLocation = {};

    UPROPERTY()
    FVector LandLocation = {};

    UPROPERTY()
    TObjectPtr<UCombatComponent> AttachComponent;
};

USTRUCT()
struct PROJECTLORD_API FSelectionActionOperation
{
    GENERATED_BODY()

public:

    UPROPERTY()
    TObjectPtr<const USelectionAction> Action;

    UPROPERTY()
    FSelectionActionContext Context;

};

UCLASS(Blueprintable)
class PROJECTLORD_API ALordPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ALordPlayerController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool SetPause(bool bPause, FCanUnpause CanUnpauseDelegate) override;
    virtual void OnRep_PlayerState() override;
    virtual void OnPossess(APawn* Pawn) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    TMulticastFuture<ALordPlayerState*> PlayerStateReady;
    TMulticastFuture<void> PlayerStateReadyVoid;
    TMulticastFuture<void> TeamStateReady;

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

    UFUNCTION(BlueprintCallable, Category = "Selection")
    FSelectionActionContext MakeSelectionContext(USelectionComponent* Selection = nullptr);

    void PerformSelectionAction(const USelectionAction* Action, const FSelectionActionContext& Context);

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
    void PlaceBuilding(UBuildingType* Type, int Cost, const UPlaceBuildingPurchase* Action);

    UFUNCTION(BlueprintCallable, Category = "SpellCasting")
    void CastSpell(UPlayerSpell* Spell);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inspection")
    bool ShowInspectWidget(TSubclassOf<UInspectWidget> WidgetClass, UVMLordBase* VM);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Focus")
    void SetFocusedActor(const AActor* Actor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Focus")
    void PanTo(FVector WorldPosition);

    UFUNCTION(BlueprintCallable, Category = "Toast")
    void AddToastNotification(FToastNotification Notification);

    USelectionComponent* GetSelectableUnderMouse() const;
    UCombatComponent* GetCombatUnderMouse() const;
    FVector GetWorldPositionUnderMouse() const;

    UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }

    UFUNCTION(BlueprintCallable, Category = "Cursor")
    void SetCursorMode(EMouseCursor::Type Mode);

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
    bool bWaitingForFlag;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMSelection> SelectionVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UVMSelection> HoverVM;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UPlacementComponent> PlacementComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<UPlayerSpellCastingComponent> SpellCastingComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<ULordGameHUDWidget> HUDWidget;

    // Replicated and used for opening UI once flag has been placed remotely
    UPROPERTY(Replicated)
    TObjectPtr<ARewardFlag> RecentFlag;

    virtual void OnSetPaused(bool bPaused);

    UFUNCTION(BlueprintNativeEvent, Category = "HUD")
    ULordGameHUDWidget* ConstructHUD();

    UFUNCTION(BlueprintImplementableEvent, Category = "Selection", meta = (DisplayName = "OnSetPaused"))
    void BP_OnSetPaused(bool bPaused);
    bool CanSelect(const AActor* ClickedActor) const;

    UFUNCTION(Server, Reliable)
    void Server_PlaceBuilding(FPlaceBuildingOperation PlaceOperation);

    UFUNCTION(Server, Reliable)
    void Server_CastSpell(FPlayerSpellCastOperation CastOperation);

    UFUNCTION(Server, Reliable)
    void Server_PlaceFlag(FFlagPlaceOperation FlagOperation);

    UFUNCTION(Server, Reliable)
    void Server_PerformSelectionAction(FSelectionActionOperation Operation);

    void OnSelectionChange();
    void OnHoverChange();
};
