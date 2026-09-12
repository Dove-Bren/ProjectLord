// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"
#include "Gameplay/Combat/CombatTypes.h"
#include "Gameplay/Combat/Ability/AbilityEnums.h"

#include "CombatAbility.generated.h"

class UTexture2D;
class UCombatComponent;
class UAnimMontage;
class UVMCombatAbility;

// Base class specialization for Unit Abilities, which have extra details for showing in the UI etc.
UCLASS(Blueprintable)
class PROJECTLORD_API UCombatAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:

    const FText& GetAbilityName() const { return AbilityName; }
    const FText& GetAbilityDescription() const { return AbilityDescription; }
    UTexture2D* GetAbilityIcon() const { return AbilityIcon; }
    EAbilityTargetType GetTargetType() const { return TargetType; }
    bool IsHidden() const { return bHidden; }
    FDamageTypeMap GetDamageTypeHint() const { return DamageTypeHint; }
    int GetManaCost() const { return ManaCost; }

    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    EAbilityAnimType GetAbilityAnimation() const { return AnimType; }

    UAnimMontage* GetAbilityAnimationFromOwner(EAbilityAnimType Type) const;

    UVMCombatAbility* GetOrCreateViewModel();
    

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability|Definition")
    FText AbilityName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    FText AbilityDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    TObjectPtr<UTexture2D> AbilityIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    EAbilityTargetType TargetType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    EAbilityAnimType AnimType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    FDamageTypeMap DamageTypeHint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    int ManaCost;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    bool bHidden;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Ability")
    TObjectPtr<UVMCombatAbility> ViewModel;

    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    UCombatComponent* GetOwnerComponent() const;
    
    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    UCombatComponent* GetOwnerTarget(bool bOnlyAlive = true) const;

    UFUNCTION(BlueprintCallable, Category = "Ability|Combat")
    void ReportAbilityHit(UCombatComponent* HitComponent);
};
