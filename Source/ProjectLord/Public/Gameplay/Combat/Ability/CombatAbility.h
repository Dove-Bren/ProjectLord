// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"

#include "CombatAbility.generated.h"

class UTexture2D;
class UCombatComponent;

// Base class specialization for Unit Abilities, which have extra details for showing in the UI etc.
UCLASS(Blueprintable)
class PROJECTLORD_API UCombatAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:

    const FText& GetAbilityName() const { return AbilityName; }
    const FText& GetAbilityDescription() const { return AbilityDescription; }
    const UTexture2D* GetAbilityIcon() const { return AbilityIcon; }
    bool IsHidden() const { return bHidden; }
    

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText AbilityName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText AbilityDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UTexture2D> AbilityIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bHidden;

    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    UCombatComponent* GetOwnerComponent() const;
    
    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    UCombatComponent* GetOwnerTarget(bool bOnlyAlive = true) const;

    UFUNCTION(BlueprintCallable, Category = "Ability|Combat")
    void ReportAbilityHit(UCombatComponent* HitComponent);
};
