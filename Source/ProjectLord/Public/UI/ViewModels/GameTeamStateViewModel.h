// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/GameTeam.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "GameTeamStateViewModel.generated.h"

class UVMGold;

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

    UFUNCTION(BlueprintCallable)
    void SelectCastle();

protected:
    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "PlayerState")
    EGameTeam Team;

    UPROPERTY(BlueprintReadOnly, Getter, Category = "PlayerState")
    TObjectPtr<UVMGold> GoldVM;

    UPROPERTY()
    TWeakObjectPtr<class AGameTeamState> ParentState;
};
