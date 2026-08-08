// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayUtils.generated.h"

class AActor;
class UCombatComponent;
class UWorld;

UCLASS()
class PROJECTLORD_API UGameplayUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Combat|Damage", meta = (WorldContext = "WorldContextObject"))
    static TArray<AActor*> GetActorsNear(UWorld* WorldContextObject, AActor* Center, double Radius);
    
    UFUNCTION(BlueprintPure, Category = "Combat|Damage", meta = (WorldContext = "World"))
    static TArray<AActor*> GetActorsNearLocation(UWorld* World, const FVector& Center, double Radius);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage", meta = (WorldContext = "World"))
    static TArray<UCombatComponent*> GetCombatComponentsNearLocation(UWorld* World, const FVector& Center, double Radius);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage", meta = (WorldContext = "WorldContextObject"))
    static TArray<UCombatComponent*> GetCombatComponentsNear(UWorld* WorldContextObject, UCombatComponent* Center, double Radius);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage", meta = (WorldContext = "World"))
    static UCombatComponent* GetNearestCombatComponentNearLocation(UWorld* World, const FVector& Center, double Radius, UCombatComponent* Ignore = nullptr);

    static UCombatComponent* GetNearestCombatComponentNearLocationEx(UWorld* World, const FVector& Center, double Radius, TFunction<bool(const UCombatComponent*)> Filter);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage", meta = (WorldContext = "WorldContextObject"))
    static UCombatComponent* GetNearestCombatComponentNear(UWorld* WorldContextObject, UCombatComponent* Center, double Radius);
};
