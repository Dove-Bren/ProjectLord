// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameLevelSettings.generated.h"

class UUnitType;

UCLASS()
class PROJECTLORD_API AGameLevelSettings : public AActor
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "LevelSettings")
    int GetStartingGold() const { return StartingGold; }

    UFUNCTION(BlueprintPure, Category = "LevelSettings")
    int GetMonsterGracePeriod() const { return MonsterGracePeriod; }

    UFUNCTION(BlueprintPure, Category = "LevelSettings")
    double GetMonsterActivationPerDay() const { return MonsterActivationPerDay; }

    UFUNCTION(BlueprintPure, Category = "LevelSettings")
    float GetMonsterSwarmChance() const { return MonsterSwarmChance; }

    UFUNCTION(BlueprintPure, Category = "LevelSettings")
    int GetMonsterSwarmCount() const { return MonsterSwarmCount; }

    UFUNCTION(BlueprintPure, Category = "LevelSettings")
    const TArray<UUnitType*> &GetMonsterSwarmTypes() const { return MonsterSwarmTypes; }

protected:

    // How much gold each player should start with
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSettings|Player")
    int StartingGold = 2000;

    // How many days before monsters spawn from spawner activation, swarms, etc.
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSettings|Monsters")
    int MonsterGracePeriod = 2;

    // How much distance from a palace that spawning buildings start activating per day,
    // in unreal-units.
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSettings|Spawners")
    double MonsterActivationPerDay = 500;

    // Chance to spawn edge monsters per day. 0 <= x <= 1. O disables edge swarm spawning.
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSettings|Swarm")
    float MonsterSwarmChance = 0.0f;

    // When spawning edge swarmers, how many to spawn
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSettings|Swarm")
    int MonsterSwarmCount = 2;

    // What monsters to spawn as swarmers
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSettings|Swarm")
    TArray<UUnitType*> MonsterSwarmTypes;
};
