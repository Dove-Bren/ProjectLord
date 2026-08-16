// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Gameplay/GameTeam.h"
#include "GameplayUtils.generated.h"

class AActor;
class UCombatComponent;
class UWorld;
class UBuildingType;
class ABuilding;

UCLASS()
class PROJECTLORD_API UGameplayUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Find Actors", meta = (WorldContext = "WorldContextObject"))
    static TArray<AActor*> GetActorsNear(const UObject* WorldContextObject, AActor* Center, double Radius);
    
    UFUNCTION(BlueprintCallable, Category = "Find Actors", meta = (WorldContext = "WorldContextObject"))
    static TArray<AActor*> GetActorsNearLocation(const UObject* WorldContextObject, const FVector& Center, double Radius);

    UFUNCTION(BlueprintCallable, Category = "Find Actors", meta = (WorldContext = "WorldContextObject"))
    static TArray<UCombatComponent*> GetCombatComponentsNearLocation(const UObject* WorldContextObject, const FVector& Center, double Radius);

    UFUNCTION(BlueprintCallable, Category = "Find Actors", meta = (WorldContext = "WorldContextObject"))
    static TArray<UCombatComponent*> GetCombatComponentsNear(const UObject* WorldContextObject, UCombatComponent* Center, double Radius);

    UFUNCTION(BlueprintCallable, Category = "Find Actors", meta = (WorldContext = "WorldContextObject"))
    static UCombatComponent* GetNearestCombatComponentNearLocation(const UObject* WorldContextObject, const FVector& Center, double Radius, UCombatComponent* Ignore = nullptr);

    static UCombatComponent* GetNearestCombatComponentNearLocationEx(UWorld* World, const FVector& Center, double Radius, TFunction<bool(const UCombatComponent*)> Filter);

    UFUNCTION(BlueprintCallable, Category = "Find Actors", meta = (WorldContext = "WorldContextObject"))
    static UCombatComponent* GetNearestCombatComponentNear(const UObject* WorldContextObject, UCombatComponent* Center, double Radius);

    UFUNCTION(BlueprintCallable, Category = "Find Actors", meta = (WorldContext = "WorldContextObject"))
    static ABuilding* GetNearestBuilding(const UObject* WorldContextObject, const FVector& Center, const UBuildingType* Type, bool bRestrictType, EGameTeam Team, bool bRestrictTeam, double Radius, bool bRestrictRange);

    UFUNCTION(BlueprintCallable, Category = "Find Actors", meta = (WorldContext = "WorldContextObject"))
    static ABuilding* GetNearestBuildingOfType(const UObject* WorldContextObject, const FVector& Center, const UBuildingType* Type);

    static ABuilding* GetNearestBuildingEx(UWorld* World, const FVector& Center, TFunction<bool(const ABuilding*)> Filter);
};
