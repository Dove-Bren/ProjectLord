// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "Gameplay/GameTeam.h"

#include "BuildingViewModel.generated.h"

class UTexture2D;
class UVMLevel;
class UVMGold;
class UVMCombatData;
class UVMBuildingGoods;
class UVMResidents;
class UVMVisitors;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMBuilding : public UVMLordBase
{
    GENERATED_BODY()

public:
    
    FText GetName() const { return Name; }
    void SetName(FText InName) { UE_MVVM_SET_PROPERTY_VALUE(Name, InName); }

    FText GetDescription() const { return Description; }
    void SetDescription(FText InDescription) { UE_MVVM_SET_PROPERTY_VALUE(Description, InDescription); }

    FText GetCustomName() const { return CustomName; }
    void SetCustomName(FText InCustomName) { UE_MVVM_SET_PROPERTY_VALUE(CustomName, InCustomName); }

    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

    EGameTeam GetTeam() const { return Team; }
    void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

    UVMLevel* GetLevelVM() const { return LevelVM; }
    void SetLevelVM(UVMLevel* InLevelVM) { UE_MVVM_SET_PROPERTY_VALUE(LevelVM, InLevelVM); }

    UVMGold* GetGoldVM() const { return GoldVM; }
    void SetGoldVM(UVMGold* InGoldVM) { UE_MVVM_SET_PROPERTY_VALUE(GoldVM, InGoldVM); }

    UVMCombatData* GetCombatVM() const { return CombatVM; }
    void SetCombatVM(UVMCombatData* InCombatVM) { UE_MVVM_SET_PROPERTY_VALUE(CombatVM, InCombatVM); }

    UVMBuildingGoods* GetGoodsVM() const { return GoodsVM; }
    void SetGoodsVM(UVMBuildingGoods* InGoodsVM) { UE_MVVM_SET_PROPERTY_VALUE(GoodsVM, InGoodsVM); }

    UVMResidents* GetResidentsVM() const { return ResidentsVM; }
    void SetResidentsVM(UVMResidents* InResidentsVM) { UE_MVVM_SET_PROPERTY_VALUE(ResidentsVM, InResidentsVM); }

    UVMVisitors* GetVisitorsVM() const { return VisitorsVM; }
    void SetVisitorsVM(UVMVisitors* InVisitorsVM) { UE_MVVM_SET_PROPERTY_VALUE(VisitorsVM, InVisitorsVM); }

protected:
    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    FText Name;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    FText Description;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    FText CustomName;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    EGameTeam Team;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    TObjectPtr<UVMLevel> LevelVM;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    TObjectPtr<UVMGold> GoldVM;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    TObjectPtr<UVMCombatData> CombatVM;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    TObjectPtr<UVMBuildingGoods> GoodsVM;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    TObjectPtr<UVMResidents> ResidentsVM;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Building")
    TObjectPtr<UVMVisitors> VisitorsVM;
};
