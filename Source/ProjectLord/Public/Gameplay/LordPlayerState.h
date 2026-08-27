// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "Gameplay/GameTeam.h"

#include "LordPlayerState.generated.h"

class UVMLordPlayerState;

UCLASS(BlueprintType)
class PROJECTLORD_API ALordPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ALordPlayerState();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure)
    EGameTeam GetPlayerTeam() const { return PlayerTeam; }

    UFUNCTION(BlueprintPure)
    UVMLordPlayerState* GetViewModel() const { return ViewModel; }

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    EGameTeam PlayerTeam;

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<UVMLordPlayerState> ViewModel;
};
