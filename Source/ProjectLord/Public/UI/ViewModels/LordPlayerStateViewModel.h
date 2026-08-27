// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/GameTeam.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "LordPlayerStateViewModel.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UVMLordPlayerState : public UVMLordBase
{
    GENERATED_BODY()

public:
    UVMLordPlayerState();

    void Setup(class ALordPlayerState* State);

    EGameTeam GetTeam() const { return Team; }
    void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

protected:
    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "PlayerState")
    EGameTeam Team;

    UPROPERTY()
    TWeakObjectPtr<class ALordPlayerState> ParentState;
};
