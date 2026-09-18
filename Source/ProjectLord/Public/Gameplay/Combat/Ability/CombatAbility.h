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

enum class ECreatureCategory : uint8;

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
    TOptional<ECreatureCategory> GetRequiredTargetCategory() const { return bRequireCreatureCategory ? TOptional<ECreatureCategory>(TargetCategory) : NullOpt; }

    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    EAbilityAnimType GetAbilityAnimation() const { return AnimType; }

    UAnimMontage* GetAbilityAnimationFromOwner(EAbilityAnimType Type) const;

    UVMCombatAbility* GetOrCreateViewModel();

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;
    virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const override;
    virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
    virtual UGameplayEffect* GetCostGameplayEffect() const override;

    virtual bool CheckTargetValid(const AActor* Target) const;
    
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR  

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition", meta = (EditCondition = "bRequireCreatureCategory", EditConditionHides))
    ECreatureCategory TargetCategory;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    int ManaCost;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    bool bHidden;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Definition")
    bool bRequireCreatureCategory;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Ability")
    TObjectPtr<UVMCombatAbility> ViewModel;

    //UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Ability")
    //TObjectPtr<UGameplayEffect> ManaCostEffectOverride;

    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    UCombatComponent* GetOwnerComponent() const;
    
    UFUNCTION(BlueprintPure, Category = "Ability|Combat")
    UCombatComponent* GetOwnerTarget(bool bOnlyAlive = true) const;

    UFUNCTION(BlueprintCallable, Category = "Ability|Combat")
    void ReportAbilityHit(UCombatComponent* HitComponent);

    static UGameplayEffect* MakeManaCostGE(UObject* Outer, int Cost);
};

// Exists because uprop containers cannot have other containers, so need a struct wrapper
USTRUCT(BlueprintType)
struct PROJECTLORD_API FCombatAbilityClassArray
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
    TArray<TSubclassOf<UCombatAbility>> Array;
};
