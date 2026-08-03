// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/ResidentialBuilding.h"

#include "SpawningBuilding.generated.h"

class UUnitType;

// A building that naturally spawns creatures over time -- like a sewer or graveyard.
UCLASS(Blueprintable)
class PROJECTLORD_API ASpawningBuilding : public AResidentialBuilding
{
    GENERATED_BODY()

public:
    ASpawningBuilding();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintNativeEvent, Category = "Spawner")
    void DoSpawn();

protected:
    
    // What team spawned creatures should be. Can be different than the building's team
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Definition")
    EGameTeam SpawnTeam;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Definition")
    float SpawnPeriod = 60;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Definition")
    float SpawnPeriodVariation = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Definition")
    float SpawnChance = 1.0f;

    void ResetTimer();

    UPROPERTY(VisibleInstanceOnly, Category = "Spawnwer")
    float SpawnTimeRemaining;

    // Helper functions for basic spawning. May not be useful in child classes
    virtual UUnitType* GetTypeToSpawn() const;

};
