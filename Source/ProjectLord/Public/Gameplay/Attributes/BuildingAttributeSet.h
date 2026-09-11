// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "BuildingAttributeSet.generated.h"

UCLASS()
class PROJECTLORD_API UBuildingAttributeSet : public UCombatAttributeSet
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GoldGeneration, Category = "Attributes|Building")
    FGameplayAttributeData GoldGeneration = 0;


    UFUNCTION()
    virtual void OnRep_GoldGeneration(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ThisClass, GoldGeneration)

public:

    ATTRIBUTE_ACCESSORS_BASIC(ThisClass, GoldGeneration);
};
