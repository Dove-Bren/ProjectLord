// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/GameTeam.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "GameTeamStateViewModel.generated.h"

class UVMGold;
class UVMUnit;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMGameTeamState : public UVMLordBase
{
    GENERATED_BODY()

public:
    UVMGameTeamState();

    void Setup(class AGameTeamState* State);

    EGameTeam GetTeam() const { return Team; }
    void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

    UVMGold* GetGoldVM() const { return GoldVM; }

    TArray<UVMUnit*> GetTeamUnits() const { return TeamUnitVMs; }
    void UpdateTeamUnits(TArray<UVMUnit*> InVMs) { UE_MVVM_SET_PROPERTY_VALUE(TeamUnitVMs, InVMs); }

    UFUNCTION(BlueprintCallable)
    void SelectCastle();

protected:
    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "TeamState")
    EGameTeam Team;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "TeamState")
    TObjectPtr<UVMGold> GoldVM;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter=GetTeamUnits, Category = "TeamState")
    TArray<UVMUnit*> TeamUnitVMs;

    UPROPERTY()
    TWeakObjectPtr<class AGameTeamState> ParentState;
};
