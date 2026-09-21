// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/SpawningBuilding.h"

#include "Gameplay/LordGameState.h"
#include "Gameplay/AI/MonsterTeamController.h"
#include "Gameplay/AI/UnitController.h"
#include "Gameplay/Units/Creature.h"

ASpawningBuilding::ASpawningBuilding()
{
    SpawnTeam = Team;
    bSpawnToCapacityAtStart = false;
    bActivated = false;
    bStartActivated = false;
}

void ASpawningBuilding::BeginPlay()
{
    Super::BeginPlay();
    ResetTimer();

    if (bSpawnToCapacityAtStart)
    {
        while (DoSpawn()) {}
    }

    if (bStartActivated)
    {
        ActivateSpawner();
    }
}

void ASpawningBuilding::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (IsActivated())
    {
        if ((SpawnTimeRemaining -= DeltaSeconds) <= 0)
        {
            if (SpawnChance >= 1.0f || FMath::FRand() < SpawnChance)
            {
                DoSpawn();
            }
            ResetTimer();
        }
    }
}

void ASpawningBuilding::ActivateSpawner()
{
    if (!bActivated)
    {
        bActivated = true;
        OnActivated();
    }
}

bool ASpawningBuilding::DoSpawn_Implementation()
{
    int NumToSpawn = FMath::Max(FMath::Floor(SpawnCount), 0);
    if (FMath::Frac(SpawnCount) > 0)
    {
        if (FMath::FRand() < FMath::Frac(SpawnCount))
        {
            NumToSpawn++;
        }
    }

    bool bSuccess = false;
    while (NumToSpawn-- > 0)
    {
        bSuccess |= SpawnOneUnit();
    }

    return bSuccess;
}

bool ASpawningBuilding::SpawnOneUnit_Implementation()
{
    // Basic spawn; get a type and spawn it
    auto SpawnType = GetTypeToSpawn();
    if (!SpawnType)
    {
        return false;
    }

    auto Recruit = RecruitNewUnit(SpawnType);
    if (Recruit)
    {
        if (SpawnTeam != Team)
        {
            Recruit->SetTeam(SpawnTeam);
        }

        if (bAutoAttackSpawns)
        {
            DoUnitSwarm(Recruit);
        }

        BP_OnUnitSpawned(Recruit);
    }
    return true;
}

void ASpawningBuilding::DoUnitSwarm(AUnit* Unit)
{
    if (!ensure(EGameTeam::Monster == Unit->GetTeam()))
    {
        return;
    }

    // Get teamstate -> monster controller
    auto GameState = GetWorld()->GetGameState<ALordGameState>();
    if (!ensure(GameState))
    {
        return;
    }

    auto MonsterTeamState = GameState->GetTeam(Unit->GetTeam());
    if (!ensure(MonsterTeamState))
    {
        return;
    }

    auto MonsterController = MonsterTeamState->GetPrimaryMonsterController();
    if (!ensure(MonsterController))
    {
        return;
    }

    auto Building = MonsterController->GetNearestPlayerBuilding(GetActorLocation());
    if (Building)
    {
        Unit->GetUnitController()->OverrideTarget(Building->GetCombatComponent());
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

void ASpawningBuilding::OnActivated()
{
    ResetTimer();
    BP_OnActivated();
}
