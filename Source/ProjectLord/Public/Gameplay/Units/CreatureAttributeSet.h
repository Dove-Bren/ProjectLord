// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h" // Needed for getter/setter macros
#include "CreatureAttributeSet.generated.h"

#define DEF_REP_ATTRIBUTE(Class, Field) \
    {\
        GAMEPLAYATTRIBUTE_REPNOTIFY(Class, Field, OldValue);\
    }

UCLASS()
class PROJECTLORD_API UCreatureAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

    // How fast the unit moves
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Movement, Category = "Attributes|Characteristics")
    FGameplayAttributeData Movement = 3;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WanderRadius, Category = "Attributes|AI Characteristics")
    FGameplayAttributeData WanderRadius = 1000;



    UFUNCTION()
    virtual void OnRep_WanderRadius(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCreatureAttributeSet, WanderRadius)

    UFUNCTION()
    virtual void OnRep_Movement(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCreatureAttributeSet, Movement)

public:

    ATTRIBUTE_ACCESSORS_BASIC(UCreatureAttributeSet, Movement);
    ATTRIBUTE_ACCESSORS_BASIC(UCreatureAttributeSet, WanderRadius);
};

#undef DEF_REP_ATTRIBUTE
