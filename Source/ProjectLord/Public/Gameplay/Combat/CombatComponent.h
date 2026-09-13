// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gameplay/GameTeam.h"
#include "Gameplay/Combat/Ability/CombatAbility.h" // Including instead of forward declared to make dropdowns populate right
#include "Gameplay/Combat/CombatTypes.h"

#include "CombatComponent.generated.h"

class UCombatAttributeSet;
class UVisibleGameplayEffect;

struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;
struct FActiveGameplayEffectHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_MULTICAST_DELEGATE(FOnDeathLocal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttack, AActor*, TargetActor, UCombatComponent*, TargetCombatComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackLand, AActor*, TargetActor, UCombatComponent*, TargetCombatComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackReceived, AActor*, AttackingActor, UCombatComponent*, AttackingCombatComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuff, AActor*, TargetActor, UCombatComponent*, TargetCombatComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeal, AActor*, TargetActor, UCombatComponent*, TargetCombatComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetChange, UCombatComponent*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvulnerabilityChange, bool, bInvulnerable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectsChange, UCombatComponent*, SelfComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilitiesChange, UCombatComponent*, SelfComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, int, Health, int, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnManaChange, int, Mana, int, MaxMana);

UCLASS(BlueprintType)
class PROJECTLORD_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

    UPROPERTY(BlueprintAssignable)
    FOnDeath OnDeath;

    FOnDeathLocal OnDeathLocal;

    UPROPERTY(BlueprintAssignable)
    FOnAttack OnAttack;

    UPROPERTY(BlueprintAssignable)
    FOnAttackLand OnAttackLand;

    UPROPERTY(BlueprintAssignable)
    FOnAttackReceived OnAttackReceived;

    UPROPERTY(BlueprintAssignable)
    FOnBuff OnBuff;

    UPROPERTY(BlueprintAssignable)
    FOnHeal OnHeal;

    UPROPERTY(BlueprintAssignable)
    FOnTargetChange OnTargetChange;

    UPROPERTY(BlueprintAssignable)
    FOnInvulnerabilityChange OnInvulnerabilityChange;

    UPROPERTY(BlueprintAssignable)
    FOnEffectsChange OnEffectsChange;

    UPROPERTY(BlueprintAssignable)
    FOnHealthChange OnHealthChange;

    UPROPERTY(BlueprintAssignable)
    FOnManaChange OnManaChange;

    UPROPERTY(BlueprintAssignable)
    FOnAbilitiesChange OnAbilitiesChange;

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

    // Check for any self buffs we should apply, and try to start one.
    // Returns true if a self-buffing ability was activated.
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool TrySelfBuff();

    // Get all Unit Abilities this Unit has access to
    UFUNCTION(BlueprintPure, Category = "Combat|Ability")
    TArray<UCombatAbility*> GetCombatAbilities(bool bIncludeHidden = false) const;

    // Give a combat ability.
    // This should be used instead of giving it directly to the GAS component, since
    // this broadcasts the change.
    UFUNCTION(BlueprintCallable, Category = "Combat|Ability")
    void GiveCombatAbility(TSubclassOf<UCombatAbility> Ability);

    // Get all damage types that are used by any ability currently owned by this unit
    UFUNCTION(BlueprintPure, Category = "Combat|Ability")
    FDamageTypeMap GetRelevantDamageTypes() const;

    // Check if any abilities require mana
    UFUNCTION(BlueprintPure, Category = "Combat|Ability")
    bool GetAbilitiesRequireMana() const;



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

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Attack Received"))
    void ReceiveOnAttackReceived(AActor* TargetActor, UCombatComponent* TargetCombatComponent);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Death"))
    void ReceiveOnDeath();

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Health Changed"))
    void ReceiveOnHealthChange(int Health, int MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Mana Changed"))
    void ReceiveOnManaChange(int Mana, int MaxMana);

    UFUNCTION(BlueprintCallable, Category = "Ability")
    FGameplayAbilitySpecHandle GetPreferredAttackAbility() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
    FGameplayAbilitySpecHandle GetPreferredAbility(EAbilityTargetType TargetType) const;

    void SetTarget(UCombatComponent* InTarget);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsInvulnerable() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetInvulnerable(bool bInvulnerable);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool IsTargetable() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    UCombatComponent* GetNearestEnemy(bool bAlive = true);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    TArray<UCombatComponent*> GetRecentAttackers() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    double GetTimeOfLastCombatAction() const { return LastCombatTime; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    double GetTimeSinceLastCombatAction() const { return ensure(GetWorld()) ? (GetWorld()->GetTimeSeconds() - LastCombatTime) : 0; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    TArray<FActiveGameplayEffectHandle> GetActiveEffectHandles() const;

    // Can't be ufuncs because we have const pointers :(
    TArray<const UGameplayEffect*> GetActiveEffects() const;
    TArray<const UVisibleGameplayEffect*> GetActiveVisibleEffects() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    static UCombatComponent* GetComponentForActor(AActor* Actor);

protected:

    void MarkCombatTime();
    void ClearRecentCombatData();
    void AddRevengeTarget(UCombatComponent* RevengeTarget);
    void HandleAttackFrom(AActor* AttackingActor, UCombatComponent* AttackingCombatComponent);
    void HandleLevelChange(int NewLevel);

    void BroadcastDeath();
    void BroadcastHealthChange();
    void BroadcastManaChange();
    void BroadcastAttack(AActor* Target, UCombatComponent* TargetCombatComponent);
    void BroadcastAttackLand(AActor* Target, UCombatComponent* TargetCombatComponent);
    void BroadcastAttackReceived(AActor* AttackingActor, UCombatComponent* AttackingCombatComponent);
    void BroadcastBuff(AActor* Target, UCombatComponent* TargetCombatComponent);
    void BroadcastHeal(AActor* Target, UCombatComponent* TargetCombatComponent);

    // From the given array of available attack availabilities, select which one the unit would prefer to use.
    // The default implementation picks the most damaging ability.
    // Overrides could do something like check if a certain high-priority ability is available and use that,
    // and otherwise fall back to default.
    // Note abilities in param are shallow copies and should not be cached or mutated
    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Ability")
    const int PickPreferredAttackAbility(const TArray<FGameplayAbilitySpec>& AttackAbilities) const;

    // Abilities the owning unit will have by default
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    TArray<TSubclassOf<UCombatAbility>> DefaultAbilities;

    // Abilities that are granted at certain level intervals.
    // Note that this does NOT work if level jumps up multiple levels at once.
    UPROPERTY(EditDefaultsOnly, Category = "Hero")
    TMap<int, FCombatAbilityClassArray> LevelUpAbilities;

    // If true, component will report that it can use mana even if no abilities
    // say they require it (yet)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    bool bOverrideNeedMana = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UCombatComponent> TargetComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    TArray<UCombatComponent*> RecentRevengeTargets;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    double LastCombatTime;

    UFUNCTION()
    void OnOwnerPossessed(APawn* Pawn, AController* OldController, AController* NewController);

private:

    TOptional<FActiveGameplayEffectHandle> InvulnEffectHandle;

public:

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
