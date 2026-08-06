// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gameplay/GameTeam.h"
#include "Gameplay/Combat/Ability/CombatAbility.h" // Including instead of forward declared to make dropdowns populate right
#include "Gameplay/Combat/CombatTypes.h"

#include "CombatComponent.generated.h"

class UCombatAttributeSet;

struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;
struct FActiveGameplayEffectHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttack, AActor*, TargetActor, UCombatComponent*, TargetCombatComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackLand, AActor*, TargetActor, UCombatComponent*, TargetCombatComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetChange, UCombatComponent*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvulnerabilityChange, bool, bInvulnerable);

UCLASS(BlueprintType)
class PROJECTLORD_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

    UPROPERTY(BlueprintAssignable)
    FOnDeath OnDeath;

    UPROPERTY(BlueprintAssignable)
    FOnAttack OnAttack;

    UPROPERTY(BlueprintAssignable)
    FOnAttackLand OnAttackLand;

    UPROPERTY(BlueprintAssignable)
    FOnTargetChange OnTargetChange;

    UPROPERTY(BlueprintAssignable)
    FOnInvulnerabilityChange OnInvulnerabilityChange;

    UFUNCTION(BlueprintPure)
    bool IsDead() const;

    UFUNCTION(BlueprintPure)
    bool IsAlive() const { return !IsDead(); }

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsCloseEnoughToAttack(const UCombatComponent* OtherCombatComponent) const;

    UFUNCTION(BlueprintPure, Category = "Combat|State")
    bool CanAttack() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetDefenseFor(EDamageType InType) const;

    // TODO: Remove and make callers use attribute system?
    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetMeleeDefense() const { return GetDefenseFor(EDamageType::Melee); }

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetRangedDefense() const { return GetDefenseFor(EDamageType::Ranged); }

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetMagicDefense() const { return GetDefenseFor(EDamageType::Magic); }

    // Attack the passed in unit from this combat component
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool AttackUnit(UCombatComponent* TargetCombatComponent);

    // Get all Unit Abilities this Unit has access to
    UFUNCTION(BlueprintPure, Category = "Combat|Ability")
    TArray<UCombatAbility*> GetCombatAbilities(bool bIncludeHidden = false);


    // Convenience Accessors
    UFUNCTION(BlueprintPure, Category = "Combat")
    EGameTeam GetTeam() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    UAbilitySystemComponent* GetAbilitySubsystemComponent() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    UCombatComponent* GetCombatTarget() const { return TargetComponent; }

    UFUNCTION(BlueprintCallable, Category = "Combat Events")
    void NotifyOfAbilityHit(UCombatComponent* HitCombatComponent);

    UCombatAttributeSet* GetCombatAttributeSet() const;

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Attack"))
    void ReceiveOnAttack(AActor* TargetActor, UCombatComponent* TargetCombatComponent);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Attack Land"))
    void ReceiveOnAttackLand(AActor* TargetActor, UCombatComponent* TargetCombatComponent);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Death"))
    void ReceiveOnDeath();

    UFUNCTION(BlueprintNativeEvent, Category = "Ability")
    FGameplayAbilitySpecHandle GetPreferredAttackAbility() const;

    void SetTarget(UCombatComponent* InTarget);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsInvulnerable() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetInvulnerable(bool bInvulnerable);


protected:

    void BroadcastDeath();
    void BroadcastAttack(AActor* Target, UCombatComponent* TargetCombatComponent);
    void BroadcastAttackLand(AActor* Target, UCombatComponent* TargetCombatComponent);

    // From the given array of available attack availabilities, select which one the unit would prefer to use.
    // The default implementation picks the most damaging ability.
    // Overrides could do something like check if a certain high-priority ability is available and use that,
    // and otherwise fall back to default.
    // Note abilities in param are shallow copies and should not be cached or mutated
    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Ability")
    const int PickPreferredAttackAbility(const TArray<FGameplayAbilitySpec>& AttackAbilities) const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    TArray<TSubclassOf<UCombatAbility>> DefaultAbilities;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UCombatComponent> TargetComponent;

    UFUNCTION()
    void OnOwnerPossessed(APawn* Pawn, AController* OldController, AController* NewController);

private:

    TOptional<FActiveGameplayEffectHandle> InvulnEffectHandle;

public:

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;

};
