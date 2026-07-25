// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h" // Needed for getter/setter macros

#include "LordHeroAttributeSet.generated.h"

class ULordUnitAttributeSet;

#define DEF_REP_ATTRIBUTE(Class, Field) \
    {\
        GAMEPLAYATTRIBUTE_REPNOTIFY(Class, Field, OldValue);\
    }

UCLASS()
class PROJECTLORD_API ULordHeroAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

protected:

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Attributes|Stats")
    FGameplayAttributeData MaxMana = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes|Stats")
    FGameplayAttributeData Mana = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExtraHealthPerLevel, Category = "Attributes|Growth")
    FGameplayAttributeData StartingHealth = 10;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExtraHealthPerLevel, Category = "Attributes|Growth")
    FGameplayAttributeData ExtraHealthPerLevel = 10;

    // Affects melee damage
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Attributes|Characteristics")
    FGameplayAttributeData Strength = 1;

    // Affects ranged damage
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Agility, Category = "Attributes|Characteristics")
    FGameplayAttributeData Agility = 1;

    // Affects magic damage + mana pool
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Attributes|Characteristics")
    FGameplayAttributeData Intelligence = 1;

    // Affects max health
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Attributes|Characteristics")
    FGameplayAttributeData Stamina = 1;

    UFUNCTION()
    virtual void OnRep_MaxMana(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordHeroAttributeSet, MaxMana)

    UFUNCTION()
    virtual void OnRep_Mana(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordHeroAttributeSet, Mana)

    UFUNCTION()
    virtual void OnRep_StartingHealth(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordHeroAttributeSet, StartingHealth)

     UFUNCTION()
    virtual void OnRep_ExtraHealthPerLevel(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordHeroAttributeSet, ExtraHealthPerLevel)

    UFUNCTION()
    virtual void OnRep_Strength(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordHeroAttributeSet, Strength)

    UFUNCTION()
    virtual void OnRep_Agility(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordHeroAttributeSet, Agility)

    UFUNCTION()
    virtual void OnRep_Intelligence(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordHeroAttributeSet, Intelligence)

    UFUNCTION()
    virtual void OnRep_Stamina(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(ULordHeroAttributeSet, Stamina)

public:

    ATTRIBUTE_ACCESSORS_BASIC(ULordHeroAttributeSet, MaxMana);
    ATTRIBUTE_ACCESSORS_BASIC(ULordHeroAttributeSet, Mana);

    ATTRIBUTE_ACCESSORS_BASIC(ULordHeroAttributeSet, StartingHealth);
    ATTRIBUTE_ACCESSORS_BASIC(ULordHeroAttributeSet, ExtraHealthPerLevel);

    ATTRIBUTE_ACCESSORS_BASIC(ULordHeroAttributeSet, Strength);
    ATTRIBUTE_ACCESSORS_BASIC(ULordHeroAttributeSet, Agility);
    ATTRIBUTE_ACCESSORS_BASIC(ULordHeroAttributeSet, Intelligence);
    ATTRIBUTE_ACCESSORS_BASIC(ULordHeroAttributeSet, Stamina);

    virtual void Init(ULordUnitAttributeSet* UnitAttribs, bool bInitUnitValues = true);
    virtual void UpdateAfterLevelup();

protected:

    UPROPERTY(VisibleAnywhere)
    TWeakObjectPtr<ULordUnitAttributeSet> UnitAttribs;

    int CalculateBaseHealth() const;
    int CalculateBaseMana() const;

    void ResetBaseHealth();
    void ResetBaseMana();

};

#undef DEF_REP_ATTRIBUTE
