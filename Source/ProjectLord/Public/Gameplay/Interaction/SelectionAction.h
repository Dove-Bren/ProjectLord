// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"

#include "Gameplay/GameGood.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpell.h"

#include "SelectionAction.generated.h"

class UTexture2D;
class ALordPlayerState;
class USelectionComponent;
class AUnit;
class ABuilding;
class UUnitType;
class UBuildingType;
class UVMSelectionaction;
class AGameTeamState;
class ARewardFlag;

UENUM(BlueprintType)
enum class ESelectionActionFailureReason : uint8
{
    None,
    QueueFull,
    GuildFull,
    NeedLevel2,
    NeedLevel3,
    BuildingInProgress,
    RepairInProgress,
    CantAfford,
};

USTRUCT(BlueprintType)
struct PROJECTLORD_API FSelectionActionContext
{
    GENERATED_BODY()

public:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Selection|Action")
    TObjectPtr<AGameTeamState> TeamState;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Selection|Action")
    TObjectPtr<ALordPlayerState> PlayerState;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Selection|Action")
    TObjectPtr<USelectionComponent> Selection;
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API USelectionAction : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    //virtual void Setup(const FSelectionActionContext& Context);

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Selection|Action")
    bool IsHidden(const FSelectionActionContext& Context) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Selection|Action")
    bool CanPerform(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection|Action")
    bool Perform(const FSelectionActionContext& Context) const;

    //UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection|Action")
    //void RefreshToShow();

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    FText GetName() const { return Name; }

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    FText GetDescription() const { return Description; }

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    UTexture2D* GetIcon() const { return Icon; }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection|Action")
    UVMSelectionAction* MakeViewModel(UObject* Outer, const FSelectionActionContext& Context) const;

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Basic")
    FText Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Basic")
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Basic")
    TObjectPtr<UTexture2D> Icon;

    virtual UVMSelectionAction* ConstructBaseVM(UObject* Outer) const;
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API USelectionPurchase : public USelectionAction
{
    GENERATED_BODY()

public:

    virtual UVMSelectionAction* MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& Context) const override;
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    int GetGoldCost() const { return GoldCost; }


protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Purchase")
    int GoldCost;

    UFUNCTION(BlueprintCallable, Category = "Selection|Action")
    bool DeductGoldCost(const FSelectionActionContext& Context) const;

    /*UFUNCTION(BlueprintCallable, Category = "Selection|Action")
    void SetGoldCost(int InCost);*/ // TODO remove
};

// A purchase action that's on a unit directly. Note these are NOT actions on
// a guild to recruit one of these heroes.
UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UUnitBasedPurchase : public USelectionPurchase
{
    GENERATED_BODY()

public:

    virtual UVMSelectionAction* MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& Context) const override;
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    AUnit* GetUnit(const FSelectionActionContext& Context) const;
};

// A purchase action that's on a reward flag.
UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UFlagBasedPurchase: public USelectionPurchase
{
    GENERATED_BODY()

public:

    virtual UVMSelectionAction* MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& Context) const override;
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    ARewardFlag* GetFlag(const FSelectionActionContext& Context) const;
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UBuildingBasedPurchase : public USelectionPurchase
{
    GENERATED_BODY()

public:

    virtual UVMSelectionAction* MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& Context) const override;
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    ABuilding* GetBuilding(const FSelectionActionContext& Context) const;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Purchase")
    int RequiredBuildingLevel = 0;
};

UCLASS(Blueprintable, HideCategories = ("Selection|Action|Basic"))
class PROJECTLORD_API UResearchGoodPurchase : public UBuildingBasedPurchase
{
    GENERATED_BODY()

public:

    // Make sure building doesn't already have it
    virtual UVMSelectionAction* MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& Context) const override;
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;
    virtual bool IsHidden_Implementation(const FSelectionActionContext& Context) const;

    virtual bool Perform_Implementation(const FSelectionActionContext& Context) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action|Research")
    FGoodOffer GetGood() const { return Good; }

    virtual void PostInitProperties() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Research")
    FGoodOffer Good;
};

UCLASS(Blueprintable, HideCategories = ("Selection|Action|Basic"))
class PROJECTLORD_API UResearchPlayerSpellPurchase : public UBuildingBasedPurchase
{
    GENERATED_BODY()

public:

    // Make sure building doesn't already have it
    virtual UVMSelectionAction* MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& Context) const override;
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;
    virtual bool IsHidden_Implementation(const FSelectionActionContext& Context) const;

    virtual bool Perform_Implementation(const FSelectionActionContext& Context) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action|Research")
    UPlayerSpell* GetSpell() const { return Spell; }

    virtual void PostInitProperties() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Research")
    TObjectPtr<UPlayerSpell> Spell;
};

UCLASS(Blueprintable, HideCategories = ("Selection|Action|Basic"))
class PROJECTLORD_API UPlaceBuildingPurchase : public USelectionPurchase
{
    GENERATED_BODY()

public:

    // Check for any special reqirements based on the building type
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;

    virtual bool Perform_Implementation(const FSelectionActionContext& Context) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action|Place")
    UBuildingType* GetBuildingType() const { return BuildingType; }

    UFUNCTION(BlueprintCallable, Category = "Selection|Action|Place")
    int CalculatePrice(const FSelectionActionContext& Context) const;

    virtual void PostInitProperties() override;
    virtual UVMSelectionAction* MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& Context) const override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Place")
    TObjectPtr<UBuildingType> BuildingType;

    // Cost is actually CostExponentBase^(n-1) * Cost
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Place")
    float CostExponentBase = 1.5f;

    UPROPERTY(BlueprintReadOnly, Category = "Selection|Action|Place")
    int BaseCost;
};

UCLASS(Blueprintable, HideCategories = ("Selection|Action|Basic"))
class PROJECTLORD_API URecruitUnitPurchase : public UBuildingBasedPurchase
{
    GENERATED_BODY()

public:

    // Make sure building has room
    virtual UVMSelectionAction* MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& Context) const override;
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;

    virtual bool Perform_Implementation(const FSelectionActionContext& Context) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action|Recruit")
    UUnitType* GetUnitType() const { return UnitType; }

    virtual void PostInitProperties() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Recruit")
    TObjectPtr<UUnitType> UnitType;
};

