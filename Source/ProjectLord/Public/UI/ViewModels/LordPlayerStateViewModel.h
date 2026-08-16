// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/GameTeam.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "LordPlayerStateViewModel.generated.h"

class UVMGold;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMLordPlayerState : public UVMLordBase
{
    GENERATED_BODY()

public:
    UVMLordPlayerState();

    void Setup(class ALordPlayerState* State);

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
    TWeakObjectPtr<class ALordPlayerState> ParentState;
};
