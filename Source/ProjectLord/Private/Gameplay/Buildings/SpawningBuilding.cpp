// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/SpawningBuilding.h"

#include "Gameplay/Units/Creature.h"

ASpawningBuilding::ASpawningBuilding()
{
    SpawnTeam = Team;
}

void ASpawningBuilding::BeginPlay()
{
    Super::BeginPlay();
    ResetTimer();
}

void ASpawningBuilding::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if ((SpawnTimeRemaining -= DeltaSeconds) <= 0)
    {
        if (SpawnChance >= 1.0f || FMath::FRand() < SpawnChance)
        {
            DoSpawn();
        }
        ResetTimer();
    }
}

void ASpawningBuilding::DoSpawn_Implementation()
{
    // Basic spawn; get a type and spawn it
    auto SpawnType = GetTypeToSpawn();
    if (!SpawnType)
    {
        return;
    }

    auto Recruit = RecruitNewUnit(SpawnType);
    if (Recruit && SpawnTeam != Team)
    {
        Recruit->SetTeam(SpawnTeam);
    }
}

void ASpawningBuilding::ResetTimer()
{
    SpawnTimeRemaining = SpawnPeriod + FMath::FRandRange(-SpawnPeriodVariation, SpawnPeriodVariation);
}

UUnitType* ASpawningBuilding::GetTypeToSpawn() const
{
    // Look at configured resident types and maxes and see what we have room for
    TArray<UUnitType*> EligibleTypes;
    for (auto& TypeLimit : ResidentTypeLimits)
    {
        if (CanFitResidentType(TypeLimit.Key))
        {
            EligibleTypes.Add(TypeLimit.Key);
        }
    }

    if (EligibleTypes.IsEmpty())
    {
        return nullptr;
    }

    return EligibleTypes[FMath::RandRange(0, EligibleTypes.Num() - 1)];
}
