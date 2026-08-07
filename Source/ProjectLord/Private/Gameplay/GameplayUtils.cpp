// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/GameplayUtils.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"

#include "Gameplay/Combat/CombatComponent.h"

/*static*/ TArray<AActor*> UGameplayUtils::GetActorsNear(AActor* Center, double Radius)
{
    auto Actors = GetActorsNearLocation(Center->GetWorld(), Center->GetActorLocation(), Radius);
    Actors.Remove(Center);
    return Actors;
}

/*static*/ TArray<AActor*> UGameplayUtils::GetActorsNearLocation(UWorld* World, const FVector& Center, double Radius)
{
    TArray<AActor*> Results;
    FCollisionShape Collider = FCollisionShape::MakeSphere(Radius);
    FCollisionQueryParams Params;
    TArray<FOverlapResult> Overlaps;
    if (World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity, ECC_Pawn, Collider, Params))
    {
        for (auto& Result : Overlaps)
        {
            Results.Add(Result.GetActor());
        }
    }

    return Results;
}

/*static*/ TArray<UCombatComponent*> UGameplayUtils::GetCombatComponentsNearLocation(UWorld* World, const FVector& Center, double Radius)
{
    TArray<UCombatComponent*> Results;
    auto NearbyActors = GetActorsNearLocation(World, Center, Radius);
    for (auto Actor : NearbyActors)
    {
        if (UCombatComponent* Combat = Actor->GetComponentByClass<UCombatComponent>())
        {
            Results.Add(Combat);
        }
    }
    return Results;
}

/*static*/ TArray<UCombatComponent*> UGameplayUtils::GetCombatComponentsNear(UCombatComponent* Center, double Radius)
{
    auto Results = GetCombatComponentsNearLocation(Center->GetWorld(), Center->GetOwner()->GetActorLocation(), Radius);
    Results.Remove(Center);
    return Results;
}

/*static*/ UCombatComponent* UGameplayUtils::GetNearestCombatComponentNearLocation(UWorld* World, const FVector& Center, double Radius, UCombatComponent* Ignore)
{
    auto All = GetCombatComponentsNearLocation(World, Center, Radius);
    if (Ignore)
    {
        All.Remove(Ignore);
    }

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

/*static*/ UCombatComponent* UGameplayUtils::GetNearestCombatComponentNear(UCombatComponent* Center, double Radius)
{
    return GetNearestCombatComponentNearLocation(Center->GetWorld(), Center->GetOwner()->GetActorLocation(), Radius, Center);
}
