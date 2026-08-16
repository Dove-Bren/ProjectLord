// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/GameplayUtils.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"
#include "EngineUtils.h" // For actor iterator

#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Combat/CombatComponent.h"

/*static*/ TArray<AActor*> UGameplayUtils::GetActorsNear(const UObject* WorldContextObject, AActor* Center, double Radius)
{
    auto Actors = GetActorsNearLocation(Center->GetWorld(), Center->GetActorLocation(), Radius);
    Actors.Remove(Center);
    return Actors;
}

/*static*/ TArray<AActor*> UGameplayUtils::GetActorsNearLocation(const UObject* WorldContextObject, const FVector& Center, double Radius)
{
    TArray<AActor*> Results;
    FCollisionShape Collider = FCollisionShape::MakeSphere(Radius);
    FCollisionQueryParams Params;
    TArray<FOverlapResult> Overlaps;
    if (WorldContextObject->GetWorld()->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity, ECC_Pawn, Collider, Params))
    {
        for (auto& Result : Overlaps)
        {
            Results.AddUnique(Result.GetActor());
        }
    }

    return Results;
}

/*static*/ TArray<UCombatComponent*> UGameplayUtils::GetCombatComponentsNearLocation(const UObject* WorldContextObject, const FVector& Center, double Radius)
{
    TArray<UCombatComponent*> Results;
    auto NearbyActors = GetActorsNearLocation(WorldContextObject, Center, Radius);
    for (auto Actor : NearbyActors)
    {
        if (UCombatComponent* Combat = Actor->GetComponentByClass<UCombatComponent>())
        {
            Results.Add(Combat);
        }
    }
    return Results;
}

/*static*/ TArray<UCombatComponent*> UGameplayUtils::GetCombatComponentsNear(const UObject* WorldContextObject, UCombatComponent* Center, double Radius)
{
    auto Results = GetCombatComponentsNearLocation(Center->GetWorld(), Center->GetOwner()->GetActorLocation(), Radius);
    Results.Remove(Center);
    return Results;
}

/*static*/ UCombatComponent* UGameplayUtils::GetNearestCombatComponentNearLocation(const UObject* WorldContextObject, const FVector& Center, double Radius, UCombatComponent* Ignore)
{
    return GetNearestCombatComponentNearLocationEx(WorldContextObject->GetWorld(), Center, Radius, [Ignore](const UCombatComponent* Other) -> bool {
                return Ignore ? Other == Ignore : false;
        });
}

/*static*/ UCombatComponent* UGameplayUtils::GetNearestCombatComponentNearLocationEx(UWorld* World, const FVector& Center, double Radius, TFunction<bool(const UCombatComponent*)> Filter)
{
    auto All = GetCombatComponentsNearLocation(World, Center, Radius);
    All.RemoveAll(Filter);

    UCombatComponent* Nearest = nullptr;
    double NearestDistSqr = MAX_dbl;
    for (auto Combat : All)
    {
        const double DistSqr = FVector::DistSquared(Combat->GetOwner()->GetActorLocation(), Center);
        if (DistSqr < NearestDistSqr)
        {
            NearestDistSqr = DistSqr;
            Nearest = Combat;
        }
    }

    return Nearest;
}

/*static*/ UCombatComponent* UGameplayUtils::GetNearestCombatComponentNear(const UObject* WorldContextObjectContextObject, UCombatComponent* Center, double Radius)
{
    return GetNearestCombatComponentNearLocation(Center->GetWorld(), Center->GetOwner()->GetActorLocation(), Radius, Center);
}

/*static*/ ABuilding* UGameplayUtils::GetNearestBuilding(const UObject* WorldContextObject, const FVector& Center, const UBuildingType* Type, bool bRestrictType, EGameTeam Team, bool bRestrictTeam, double Radius, bool bRestrictRange)
{
    const double RadiusSqr = Radius * Radius;
    auto Filter = [Center, Type, bRestrictType, Team, bRestrictTeam, RadiusSqr, bRestrictRange](const ABuilding* Building) -> bool {
        if (bRestrictType && Building->GetBuildingType() != Type)
        {
            return true;
        }
        if (bRestrictTeam && Building->GetTeam() != Team)
        {
            return true;
        }
        if (bRestrictRange && FVector::DistSquared(Building->GetActorLocation(), Center) > RadiusSqr)
        {
            return true;
        }
        return false;
    };
    return GetNearestBuildingEx(WorldContextObject->GetWorld(), Center, Filter);
}

/*static*/ ABuilding* UGameplayUtils::GetNearestBuildingOfType(const UObject* WorldContextObject, const FVector& Center, const UBuildingType* Type)
{
    return GetNearestBuilding(WorldContextObject, Center, Type, true, {}, false, 0, false);
}

/*static*/ ABuilding* UGameplayUtils::GetNearestBuildingEx(UWorld* World, const FVector& Center, TFunction<bool(const ABuilding*)> Filter)
{
    ABuilding* Nearest = nullptr;
    double NearestRangeSqr = MAX_dbl;
    for (TActorIterator<ABuilding> Iter(World); Iter; ++Iter)
    {
        ABuilding* Building = *Iter;

        // Do range first instead of dispatching to filter
        const double Dist = FVector::DistSquared(Building->GetActorLocation(), Center);
        if (Dist >= NearestRangeSqr)
        {
            continue;
        }

        if (Filter(Building))
        {
            continue;
        }

        Nearest = Building;
        NearestRangeSqr = Dist;
    }

    return Nearest;
}
