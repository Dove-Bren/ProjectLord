// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"
#include "Gameplay/Combat/Ability/AbilityEnums.h"

#include "CombatAbility.generated.h"

class UTexture2D;
class UCombatComponent;
class UAnimMontage;

// Base class specialization for Unit Abilities, which have extra details for showing in the UI etc.
UCLASS(Blueprintable)
class PROJECTLORD_API UCombatAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:

    const FText& GetAbilityName() const { return AbilityName; }
    const FText& GetAbilityDescription() const { return AbilityDescription; }
    const UTexture2D* GetAbilityIcon() const { return AbilityIcon; }
    EAbilityTargetType GetTargetType() const { return TargetType; }
    bool IsHidden() const { return bHidden; }

    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    EAbilityAnimType GetAbilityAnimation() const { return AnimType; }

    UAnimMontage* GetAbilityAnimationFromOwner(EAbilityAnimType Type) const;
    

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
    bool bHidden;

    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    UCombatComponent* GetOwnerComponent() const;
    
    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    UCombatComponent* GetOwnerTarget(bool bOnlyAlive = true) const;

    UFUNCTION(BlueprintCallable, Category = "Ability|Combat")
    void ReportAbilityHit(UCombatComponent* HitComponent);
};
