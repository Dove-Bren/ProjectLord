// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameLevelSettings.generated.h"

UCLASS()
class PROJECTLORD_API AGameLevelSettings : public AActor
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "LevelSettings")
    int GetStartingGold() const { return StartingGold; }

    UFUNCTION(BlueprintPure, Category = "LevelSettings")
    double GetMonsterActivationPerDay() const { return MonsterActivationPerDay; }

protected:

    // How much gold each player should start with
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSettings")
    int StartingGold = 500;

    // How much distance from a palace that spawning buildings start activating per day,
    // in unreal-units.
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSettings")
    double MonsterActivationPerDay = 500;
};
