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

    UFUNCTION(BlueprintPure, Category = "Combat|Damage")
    static TArray<AActor*> GetActorsNear(AActor* Center, double Radius);
    
    UFUNCTION(BlueprintPure, Category = "Combat|Damage")
    static TArray<AActor*> GetActorsNearLocation(UWorld* World, const FVector& Center, double Radius);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage")
    static TArray<UCombatComponent*> GetCombatComponentsNearLocation(UWorld* World, const FVector& Center, double Radius);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage")
    static TArray<UCombatComponent*> GetCombatComponentsNear(UCombatComponent* Center, double Radius);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage")
    static UCombatComponent* GetNearestCombatComponentNearLocation(UWorld* World, const FVector& Center, double Radius, UCombatComponent* Ignore = nullptr);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage")
    static UCombatComponent* GetNearestCombatComponentNear(UCombatComponent* Center, double Radius);
};
