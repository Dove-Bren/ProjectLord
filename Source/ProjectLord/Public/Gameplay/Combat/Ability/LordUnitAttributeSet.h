// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h" // Needed for getter/setter macros
#include "LordUnitAttributeSet.generated.h"

#define DEF_REP_ATTRIBUTE(Class, Field) \
    {\
        GAMEPLAYATTRIBUTE_REPNOTIFY(Class, Field, OldValue);\
    }

UCLASS()
class PROJECTLORD_API ULordUnitAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

protected:

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Level, Category = "Attributes|Stats")
    FGameplayAttributeData Level = 1;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes|Stats")
    FGameplayAttributeData MaxHealth = 10;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes|Stats")
    FGameplayAttributeData Health = 10;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MeleeDefense, Category = "Attributes|Defense")
    FGameplayAttributeData MeleeDefense = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RangedDefense, Category = "Attributes|Defense")
    FGameplayAttributeData RangedDefense = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicDefense, Category = "Attributes|Defense")
    FGameplayAttributeData MagicDefense = 0;

    // How fast the unit moves
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Movement, Category = "Attributes|Characteristics")
    FGameplayAttributeData Movement = 3;

    // Bonus melee damage, like from equipment or class abilities
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BonusMeleeDamage, Category = "Attributes|Combat Modifiers")
    FGameplayAttributeData BonusMeleeDamage = 0;

    // Bonus rannged damage, like from equipment or class abilities
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BonusRangedDamage, Category = "Attributes|Combat Modifiers")
    FGameplayAttributeData BonusRangedDamage = 0;

    // Bonus magic damage, like from equipment or class abilities
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BonusMagicDamage, Category = "Attributes|Combat Modifiers")
    FGameplayAttributeData BonusMagicDamage = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WanderRadius, Category = "Attributes|AI Characteristics")
    FGameplayAttributeData WanderRadius = 1000;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Sight, Category = "Attributes|AI Characteristics")
    FGameplayAttributeData Sight = 1000;

    UFUNCTION()
    virtual void OnRep_Level(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, Level)

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, MaxHealth)

    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, Health)

    UFUNCTION()
    virtual void OnRep_MeleeDefense(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, MeleeDefense)

    UFUNCTION()
    virtual void OnRep_RangedDefense(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, RangedDefense)

    UFUNCTION()
    virtual void OnRep_MagicDefense(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, MagicDefense)

    UFUNCTION()
    virtual void OnRep_Movement(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, Movement)

    UFUNCTION()
    virtual void OnRep_BonusMeleeDamage(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, BonusMeleeDamage)

    UFUNCTION()
    virtual void OnRep_BonusRangedDamage(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, BonusRangedDamage)

    UFUNCTION()
    virtual void OnRep_BonusMagicDamage(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, BonusMagicDamage)

    UFUNCTION()
    virtual void OnRep_WanderRadius(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, WanderRadius)

    UFUNCTION()
    virtual void OnRep_Sight(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordUnitAttributeSet, Sight)


public:

    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, Level);
    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, MaxHealth);
    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, Health);

    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, MeleeDefense);
    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, RangedDefense);
    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, MagicDefense);

    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, Movement);

    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, BonusMeleeDamage);
    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, BonusRangedDamage);
    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, BonusMagicDamage);

    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, WanderRadius);
    ATTRIBUTE_ACCESSORS_BASIC(ULordUnitAttributeSet, Sight);

};

#undef DEF_REP_ATTRIBUTE
