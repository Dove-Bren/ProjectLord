// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/GameplayUtils.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"

#include "Gameplay/Combat/CombatComponent.h"

/*static*/ TArray<AActor*> UGameplayUtils::GetActorsNear(UWorld* WorldContextObject, AActor* Center, double Radius)
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
            Results.AddUnique(Result.GetActor());
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

/*static*/ TArray<UCombatComponent*> UGameplayUtils::GetCombatComponentsNear(UWorld* WorldContextObject, UCombatComponent* Center, double Radius)
{
    auto Results = GetCombatComponentsNearLocation(Center->GetWorld(), Center->GetOwner()->GetActorLocation(), Radius);
    Results.Remove(Center);
    return Results;
}

/*static*/ UCombatComponent* UGameplayUtils::GetNearestCombatComponentNearLocation(UWorld* World, const FVector& Center, double Radius, UCombatComponent* Ignore)
{
    return GetNearestCombatComponentNearLocationEx(World, Center, Radius, [Ignore](const UCombatComponent* Other) -> bool {
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

/*static*/ UCombatComponent* UGameplayUtils::GetNearestCombatComponentNear(UWorld* WorldContextObject, UCombatComponent* Center, double Radius)
{
    return GetNearestCombatComponentNearLocation(Center->GetWorld(), Center->GetOwner()->GetActorLocation(), Radius, Center);
}
