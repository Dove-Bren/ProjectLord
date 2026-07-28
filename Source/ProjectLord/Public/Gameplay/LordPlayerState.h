// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "Gameplay/Units/UnitTypes.h"

#include "LordPlayerState.generated.h"

class ABuilding;

UCLASS(BlueprintType)
class PROJECTLORD_API ALordPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ALordPlayerState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure)
    EUnitTeam GetPlayerTeam() const { return PlayerTeam; }

    UFUNCTION(BlueprintPure)
    int GetPlayerGold() const { return Gold; }

    UFUNCTION(BlueprintCallable)
    int AddGold(int InChange) { Gold = FMath::Clamp(Gold + InChange, 0, 9999999); return Gold; }

    UFUNCTION(BlueprintPure)
    ABuilding* GetPlayerCastle() const { return PlayerCastle.IsValid() ? PlayerCastle.Get() : nullptr; }

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    EUnitTeam PlayerTeam;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    int Gold;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    TWeakObjectPtr<ABuilding> PlayerCastle;
};
