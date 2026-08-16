// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "Gameplay/GameTeam.h"

#include "LordPlayerState.generated.h"

class ABuilding;
class UVMLordPlayerState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerGoldChanged, int);

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
    int GetPlayerGold() const { return Gold; }

    UFUNCTION(BlueprintCallable)
    int AddGold(int InChange) { Gold = FMath::Clamp(Gold + InChange, 0, 9999999); OnPlayerGoldChanged.Broadcast(Gold); return Gold; }

    UFUNCTION(BlueprintPure)
    ABuilding* GetPlayerCastle() const { return PlayerCastle.IsValid() ? PlayerCastle.Get() : nullptr; }

    UFUNCTION(BlueprintCallable)
    void SetCastle(ABuilding* Castle);

    UFUNCTION(BlueprintPure)
    UVMLordPlayerState* GetViewModel() const { return ViewModel; }

    FOnPlayerGoldChanged OnPlayerGoldChanged;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    EGameTeam PlayerTeam;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    int Gold;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player")
    TWeakObjectPtr<ABuilding> PlayerCastle;

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<UVMLordPlayerState> ViewModel;
};
