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

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawner")
    bool DoSpawn();

    UFUNCTION(BlueprintPure, Category = "Spawner")
    bool IsActivated() const { return bActivated; }

    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void ActivateSpawner();

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

    // Number of units to spawn each period, with fractions being treated
    // as 'chance of spawning one more.'
    // Ex:  1.5 means always spawn 1, and 50% to spawn another.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Definition")
    float SpawnCount = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawner|Definition")
    bool bSpawnToCapacityAtStart;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawner|Definition")
    bool bStartActivated;

    UPROPERTY(VisibleInstanceOnly, Category = "Spawner")
    bool bActivated;

    void ResetTimer();

    UPROPERTY(VisibleInstanceOnly, Category = "Spawner")
    float SpawnTimeRemaining;

    // Helper functions for basic spawning. May not be useful in child classes
    virtual UUnitType* GetTypeToSpawn() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawner")
    bool SpawnOneUnit();

    virtual void OnActivated();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner", meta = (DisplayName = "OnActivated"))
    void BP_OnActivated();

};
