// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"

#include "LordGameMode.generated.h"

class ACastle;

// This is where we would do things like encode logic about not letting players in,
// or not having monsters, etc. This is THE game mode.
// For now, making only one and making it have the normal ref game gameplay.
// This may need to be abstracted + specialized in the future.
UCLASS(BlueprintType)
class PROJECTLORD_API ALordGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ALordGameMode();

    virtual void PostLogin(APlayerController* NewController) override;

    virtual TSubclassOf<ACastle> GetCastleForPlayer(const APlayerController* Controller) const;

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<ACastle> PlayerCastleType;

};
