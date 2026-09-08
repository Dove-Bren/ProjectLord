// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/SpawningBuilding.h"

#include "Castle.generated.h"

class UUnitType;
class UBuildingType;
class AGameTeamState;

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API ACastle : public ASpawningBuilding
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;


protected:

    // List of unit types to count as peasant
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Castle")
    TArray<const UUnitType*> PeasantTypes;

    // List of building types to count as houses
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Castle")
    TArray<const UBuildingType*> HouseTypes;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Castle|Spawning")
    float NextSpawnTime;

    UFUNCTION(BlueprintCallable, Category = "Town")
    void AttemptSpawnRound();

    UFUNCTION(BlueprintCallable, Category = "Town")
    bool SpawnNearby(const UBuildingType* Type);

    UFUNCTION(BlueprintCallable, Category = "Town")
    bool AutoPlaceBuilding(const UBuildingType* Type, FVector At);

    UFUNCTION(BlueprintPure, Category = "Town")
    int GetHouseCount() const;

    UFUNCTION(BlueprintPure, Category = "Town")
    int GetHeroCount() const;

    UFUNCTION(BlueprintPure, Category = "Town")
    int GetBuildingCount() const;

    UFUNCTION(BlueprintPure, Category = "Town")
    int GetDesiredHouseCount() const;
    UFUNCTION(BlueprintPure, Category = "Town")
    int GetDesiredTaxCollectorCount() const;
    UFUNCTION(BlueprintPure, Category = "Town")
    int GetDesiredRoyalGuardCount() const;

    UFUNCTION(BlueprintPure, Category = "Town")
    AGameTeamState* GetTeamState() const;

};
