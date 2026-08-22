// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/GameGood.h"

#include "SelectionAction.generated.h"

class UTexture2D;
class ALordPlayerState;
class USelectionComponent;
class AUnit;
class ABuilding;
class UUnitType;
class UBuildingType;

USTRUCT(BlueprintType)
struct PROJECTLORD_API FSelectionActionContext
{
    GENERATED_BODY()

public:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Selection|Action")
    ALordPlayerState* PlayerState;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Selection|Action")
    USelectionComponent* Selection;
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API USelectionAction : public UObject
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Selection|Action")
    bool IsHidden(const FSelectionActionContext& Context) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Selection|Action")
    bool CanPerform(const FSelectionActionContext& Context) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection|Action")
    bool Perform(const FSelectionActionContext& Context);

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    FText GetName() const { return Name; }

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    FText GetDescription() const { return Description; }

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    UTexture2D* GetIcon() const { return Icon; }

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action")
    FText Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action")
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action")
    TObjectPtr<UTexture2D> Icon;
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API USelectionPurchase : public USelectionAction
{
    GENERATED_BODY()

public:

    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    int GetGoldCost() const { return GoldCost; }


protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action")
    int GoldCost;
};

// A purchase action that's on a unit directly. Note these are NOT actions on
// a guild to recruit one of these heroes.
UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UUnitBasedPurchase : public USelectionPurchase
{
    GENERATED_BODY()

public:

    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    AUnit* GetUnit(const FSelectionActionContext& Context) const;
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UBuildingBasedPurchase : public USelectionPurchase
{
    GENERATED_BODY()

public:

    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context) const override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action")
    ABuilding* GetBuilding(const FSelectionActionContext& Context) const;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action")
    int RequiredBuildingLevel = 0;
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UResearchGoodPurchase : public UBuildingBasedPurchase
{
    GENERATED_BODY()

public:

    // Make sure building doesn't already have it
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context) const override;
    virtual bool IsHidden_Implementation(const FSelectionActionContext& Context) const;

    virtual bool Perform_Implementation(const FSelectionActionContext& Context) override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action|Research")
    FGoodOffer GetGood() const { return Good; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Research")
    FGoodOffer Good;
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UPlaceBuildingPurchase : public USelectionPurchase
{
    GENERATED_BODY()

public:

    // Check for any special reqirements based on the building type
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context) const override;

    virtual bool Perform_Implementation(const FSelectionActionContext& Context) override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action|Place")
    UBuildingType* GetBuildingType() const { return BuildingType; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Place")
    TObjectPtr<UBuildingType> BuildingType;
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API URecruitUnitPurchase : public UBuildingBasedPurchase
{
    GENERATED_BODY()

public:

    // Make sure building has room
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context) const override;

    virtual bool Perform_Implementation(const FSelectionActionContext& Context) override;

    UFUNCTION(BlueprintPure, Category = "Selection|Action|Recruit")
    UUnitType* GetUnitType() const { return UnitType; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Recruit")
    TObjectPtr<UUnitType> UnitType;
};

