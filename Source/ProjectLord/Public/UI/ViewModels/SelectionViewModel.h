// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "Gameplay/Combat/CombatTypes.h"

#include "SelectionViewModel.generated.h"

USTRUCT(BlueprintType)
struct PROJECTLORD_API FSelectionProperties
{
    GENERATED_BODY()

public:
    FSelectionProperties() {};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bHasGold = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bHasAction = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bHasCombatData = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bHasLevel = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bProgressQueue = false;

    static FSelectionProperties MakeChest() { FSelectionProperties Ret; Ret.bHasGold = true; return Ret; }
    static FSelectionProperties MakeCivilian() { FSelectionProperties Ret; Ret.bHasAction = true; Ret.bHasCombatData = true; return Ret; }
    static FSelectionProperties MakeHero() { FSelectionProperties Ret; Ret.bHasGold = true; Ret.bHasAction = true; Ret.bHasCombatData = true; Ret.bHasLevel = true; return Ret; }
    static FSelectionProperties MakeMonster() { FSelectionProperties Ret; Ret.bHasGold = true; Ret.bHasAction = true; Ret.bHasCombatData = true; return Ret; }
    static FSelectionProperties MakeBasicBuilding() { FSelectionProperties Ret; Ret.bHasGold = true; Ret.bHasCombatData = true; return Ret; } // Note: Buildings don't show level
    static FSelectionProperties MakeGuildBuilding() { FSelectionProperties Ret; Ret.bHasGold = true; Ret.bHasCombatData = true; Ret.bProgressQueue = true; return Ret; }

    bool operator==(const FSelectionProperties& Other) const = default;
};

// Base VM class for anything selectable
UCLASS(Abstract)
class PROJECTLORD_API USelectionViewModel : public UVMLordBase
{
    GENERATED_BODY()

public:
    
    FSelectionProperties GetSelectionProperties() const { return SelectionProperties; }
    EUnitTeam GetTeam() const { return Team; }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection")
    FSelectionProperties SelectionProperties;
    void SetSelectionProperties(FSelectionProperties InProperties) { UE_MVVM_SET_PROPERTY_VALUE(SelectionProperties, InProperties); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection")
    EUnitTeam Team;
    void SetTeam(EUnitTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

private:
    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection")
    TObjectPtr<USelectionComponent*> SelectionComponent;

};

UCLASS(Abstract)
class PROJECTLORD_API UUnitSelectionViewModel : public USelectionViewModel
{
    int GetGold() const;

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection")
    int Gold;
    void SetGold();//
};
