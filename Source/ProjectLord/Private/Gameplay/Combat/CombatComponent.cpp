// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/CombatComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"

#include "LordLogging.h"
#include "Gameplay/GameplayUtils.h"
#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/AI/UnitController.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Combat/GameplayEffect/InvulnerabilityGameplayEffect.h"
#include "Gameplay/Combat/GameplayEffect/VisibleGameplayEffect.h"
#include "Gameplay/Units/Unit.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	auto AbilitySystemComponent = GetAbilitySubsystemComponent();
	if (ensure(AbilitySystemComponent))
	{
        for (auto& Ability : DefaultAbilities)
        {
            AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
        }
        OnAbilitiesChange.Broadcast(this);

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetCombatAttributeSet()->GetHealthAttribute())
            .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
                {
                    if (ChangeData.OldValue != ChangeData.NewValue)
                    {
                        BroadcastHealthChange();
                        if (!bDead && ChangeData.NewValue <= 0)
                        {
                            bDead = true;
                            BroadcastDeath();
                        }
                    }
                });

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetCombatAttributeSet()->GetMaxHealthAttribute())
            .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
                {
                    if (ChangeData.OldValue != ChangeData.NewValue)
                    {
                        BroadcastHealthChange();
                    }
                });

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetCombatAttributeSet()->GetManaAttribute())
            .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
                {
                    if (ChangeData.OldValue != ChangeData.NewValue)
                    {
                        BroadcastManaChange();
                    }
                });

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetCombatAttributeSet()->GetMaxManaAttribute())
            .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
                {
                    if (ChangeData.OldValue != ChangeData.NewValue)
                    {
                        BroadcastManaChange();
                    }
                });

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetCombatAttributeSet()->GetLevelAttribute())
            .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
                {
                    if (ChangeData.OldValue != ChangeData.NewValue)
                    {
                        HandleLevelChange(ChangeData.NewValue);
                    }
                });



        AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddWeakLambda(this, [this](const FActiveGameplayEffect&) {
            OnEffectsChange.Broadcast(this);
        });
        AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddWeakLambda(this, [this](UAbilitySystemComponent*, const FGameplayEffectSpec&, FActiveGameplayEffectHandle) {
            OnEffectsChange.Broadcast(this);
        });
        // WAnt to listen for ability add/remove event, but there isn't one!

        // Set up modifier for threat based on level
        {
            UGameplayEffect* GE_ThreatMod = NewObject<UGameplayEffect>(this, TEXT("ThreatLevelMod"));
            GE_ThreatMod->DurationPolicy = EGameplayEffectDurationType::Infinite;
            auto ThreatAttribute = UCombatAttributeSet::GetThreatAttribute();

            FGameplayModifierInfo Mod;
            Mod.Attribute = ThreatAttribute;
            Mod.ModifierOp = EGameplayModOp::AddBase;
            FAttributeBasedFloat Curve;
            Curve.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(UCombatAttributeSet::GetLevelAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
            Curve.Coefficient = 1;
            Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Curve);
            GE_ThreatMod->Modifiers.Add(MoveTemp(Mod));

            FGameplayEffectSpec Spec(GE_ThreatMod, AbilitySystemComponent->MakeEffectContext(), 1);
            /*ConfidenceLevelModHandle = */AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(Spec);
        }
	}

    if (auto OwnerPawn = Cast<APawn>(GetOwner()))
    {
        OwnerPawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UCombatComponent::OnOwnerPossessed);
        if (auto OwnerController = OwnerPawn->GetController<AUnitController>())
        {
            OnOwnerPossessed(OwnerPawn, nullptr, OwnerController);
        }
        
    }

    LastCombatTime = 0;
}

void UCombatComponent::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);
}

void UCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (TargetComponent)
    {
        if (!IsValid(TargetComponent) || !TargetComponent->IsAlive())
        {
            SetTarget(nullptr);
        }
    }

    constexpr double CombatTimeoutSeconds = 10;
    if (LastCombatTime != 0)
    {
        const auto TimeSince = GetTimeSinceLastCombatAction();

        // If it's been long enough, clear
        if (TimeSince > CombatTimeoutSeconds)
        {
            ClearRecentCombatData();
        }
    }
}

EGameTeam UCombatComponent::GetTeam() const
{
    // Could cache a getter to avoid the conditionals most of the time, but then can't be const...
    /*if (!TeamGetter.IsSet())
    {
        auto Owner = GetOwner();
        if (auto Unit = Cast<AUnit>(Owner))
        {
            TeamGetter = [WeakUnit = TWeakObjectPtr(Unit)]() -> EGameTeam { return WeakUnit->GetTeam(); };
        }
        if (auto Building = Cast<ABuilding>(Owner))
        {
            TeamGetter = [WeakBuilding = TWeakObjectPtr(Building)]() -> EGameTeam { return WeakBuilding->GetTeam(); };
        }

        ensureMsgf(false, TEXT("Owning Actor (%s) is neither a Unit or a building"), *GetNameSafe(Owner));
        TeamGetter = []() { return EGameTeam::Monster; };
    }

    return (TeamGetter.GetValue())();*/

    auto Owner = GetOwner();
    if (auto Unit = Cast<AUnit>(Owner))
    {
        return Unit->GetTeam();
    }
    if (auto Building = Cast<ABuilding>(Owner))
    {
        return Building->GetTeam();
    }

    ensureMsgf(false, TEXT("Owning Actor (%s) is neither a Unit or a building"), *GetNameSafe(Owner));
    return EGameTeam::Monster;
}

UAbilitySystemComponent* UCombatComponent::GetAbilitySubsystemComponent() const
{
    auto Owner = GetOwner();
    return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
}

//UCombatComponent* UCombatComponent::GetCombatTarget() const
//{
//    auto Owner = GetOwner();
//    if (auto Unit = Cast<AUnit>(Owner))
//    {
//        auto UnitController = Unit->GetUnitController();
//        if (ensure(UnitController))
//        {
//            return UnitController->GetTargetComponent();
//        }
//    }
//    if (auto Building = Cast<ABuilding>(Owner))
//    {
//        // TODO
//    }
//
//    ensureMsgf(false, TEXT("Owning Actor (%s) is neither a Unit or a building"), *GetNameSafe(Owner));
//    return nullptr;
//}

void UCombatComponent::NotifyOfAbilityHit(UCombatComponent* HitCombatComponent)
{
    auto HitActor = HitCombatComponent->GetOwner();
    if (IsValid(HitActor))
    {
        BroadcastAttackLand(HitActor, HitCombatComponent);
    }
}

UCombatAttributeSet* UCombatComponent::GetCombatAttributeSet() const
{
    auto Owner = GetOwner();
    if (auto Unit = Cast<AUnit>(Owner))
    {
        return Unit->GetCombatAttributeSet();
    }
    if (auto Building = Cast<ABuilding>(Owner))
    {
        return Building->GetCombatAttributeSet();
    }

    ensureMsgf(false, TEXT("Owning Actor (%s) is neither a Unit or a building"), *GetNameSafe(Owner));
    return nullptr;
}

TArray<UCombatAbility*> UCombatComponent::GetCombatAbilities(bool bIncludeHidden) const
{
    TArray<UCombatAbility*> Abilities;

    auto AbilitySystemComponent = GetAbilitySubsystemComponent();
    if (ensure(AbilitySystemComponent))
    {
        TArray<FGameplayAbilitySpecHandle> AllAbilities;
        AbilitySystemComponent->GetAllAbilities(AllAbilities);
        for (const auto& Handle : AllAbilities)
        {
            auto Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
            if (auto Ability = Cast<UCombatAbility>(Spec->Ability))
            {
                if (bIncludeHidden || !Ability->IsHidden())
                {
                    Abilities.Add(Ability);
                }
            }
        }
    }

    return Abilities;
}

void UCombatComponent::GiveCombatAbility(TSubclassOf<UCombatAbility> Ability)
{
    auto AbilitySystemComponent = GetAbilitySubsystemComponent();
    if (ensure(AbilitySystemComponent) && !AbilitySystemComponent->FindAbilitySpecFromClass(Ability))
    {
        AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
        OnAbilitiesChange.Broadcast(this);
    }
}

FDamageTypeMap UCombatComponent::GetRelevantDamageTypes() const
{
    FDamageTypeMap Map;
    for (auto Ability : GetCombatAbilities(true))
    {
        Map.Merge(Ability->GetDamageTypeHint());
    }
    return Map;
}

bool UCombatComponent::GetAbilitiesRequireMana() const
{
    if (bOverrideNeedMana)
    {
        return true;
    }

    for (auto Ability : GetCombatAbilities(true))
    {
        if (Ability->GetManaCost() > 0)
        {
            return true;
        }
    }

    return false;
}


bool UCombatComponent::IsDead() const
{
    return bDead;
}

int UCombatComponent::GetHealth() const
{
    bool bIgnored;
    return FMath::TruncToInt(GetAbilitySubsystemComponent()->GetGameplayAttributeValue(GetCombatAttributeSet()->GetHealthAttribute(), bIgnored));
}

int UCombatComponent::GetMaxHealth() const
{
    bool bIgnored;
    return FMath::TruncToInt(GetAbilitySubsystemComponent()->GetGameplayAttributeValue(GetCombatAttributeSet()->GetMaxHealthAttribute(), bIgnored));
}

int UCombatComponent::GetRawThreat() const
{
    bool bIgnored;
    return FMath::TruncToInt(GetAbilitySubsystemComponent()->GetGameplayAttributeValue(UCombatAttributeSet::GetThreatAttribute(), bIgnored));
}

float UCombatComponent::GetThreat() const
{
    const int Raw = GetRawThreat();
    return Raw * (GetHealth() / GetMaxHealth());
}

bool UCombatComponent::IsCloseEnoughToAttack(const UCombatComponent* OtherCombatComponent) const
{
    bool bIgnored;
    auto Owner = GetOwner();
    auto Target = OtherCombatComponent->GetOwner();
    const auto AttackRange = GetAbilitySubsystemComponent()->GetGameplayAttributeValue(GetCombatAttributeSet()->GetAttackRangeAttribute(), bIgnored);
    const auto Dist = FVector::DistXY(Owner->GetActorLocation(), Target->GetActorLocation());

    // Things have different sized hitboxes. Buildings for example are large, and
    // it might not be feasible to get within X units of the _center_ of it.
    // So check distance to the outer edge of the target's size, roughly;

    float OwnerHalfWidth;
    float TargetHalfWidth;

    /*float Ignored;
    float OwnerHalfWidth = 0;
    float TargetHalfWidth = 0;
    Owner->GetSimpleCollisionCylinder(OwnerHalfWidth, Ignored);
    Target->GetSimpleCollisionCylinder(TargetHalfWidth, Ignored);*/
    if (const ABuilding* OwnerUnit = Cast<ABuilding>(Owner))
    {
        float Ignored;
        OwnerUnit->GetBuildingMeshComponent()->CalcBoundingCylinder(OwnerHalfWidth, Ignored);
    }
    else
    {
        FVector OwnerOrigin, OwnerBounds;
        Owner->GetActorBounds(true, OwnerOrigin, OwnerBounds);
        OwnerHalfWidth = (OwnerBounds.X + OwnerBounds.Y) / 2.0f;
    }

    if (const ABuilding* TargetUnit = Cast<ABuilding>(Target))
    {
        float Ignored;
        TargetUnit->GetBuildingMeshComponent()->CalcBoundingCylinder(TargetHalfWidth, Ignored);
    }
    else
    {
        FVector TargetOrigin, TargetBounds;
        Target->GetActorBounds(true, TargetOrigin, TargetBounds);
        TargetHalfWidth = (TargetBounds.X + TargetBounds.Y) / 2.0f;
    }


    return Dist <= (AttackRange + OwnerHalfWidth + TargetHalfWidth);
}

bool UCombatComponent::HasHeal() const
{
    for (auto Ability : GetCombatAbilities())
    {
        if (EAbilityTargetType::Heal == Ability->GetTargetType())
        {
            return true;
        }
    }
    return false;
}

int UCombatComponent::GetDefenseFor(EDamageType InType) const
{
    FGameplayAttribute Attribute;
    switch (InType)
    {
    case EDamageType::Melee:
        Attribute = GetCombatAttributeSet()->GetMeleeDefenseAttribute();
        break;
    case EDamageType::Ranged:
        Attribute = GetCombatAttributeSet()->GetRangedDefenseAttribute();
        break;
    case EDamageType::Magic:
        Attribute = GetCombatAttributeSet()->GetMagicDefenseAttribute();
        break;
    default:
        return 0;
    }
    bool bIgnored;
    return FMath::TruncToInt(GetAbilitySubsystemComponent()->GetGameplayAttributeValue(Attribute, bIgnored));
}

bool UCombatComponent::CanAttack() const
{
    return !GetAbilitySubsystemComponent()->HasMatchingGameplayTag(ULordGameplayTags::UnitStateAttacking());
}

bool UCombatComponent::AttackUnit(UCombatComponent* TargetCombatComponent)
{
    bool bValid;
    FGameplayAbilitySpecHandle AttackAbility = GetPreferredAttackAbility(TargetCombatComponent->GetOwner(), bValid);
    if (bValid)
    {
        if (GetAbilitySubsystemComponent()->TryActivateAbility(AttackAbility))
        {
            MarkCombatTime();
            BroadcastAttack(TargetCombatComponent->GetOwner(), TargetCombatComponent);
            TargetCombatComponent->HandleAttackFrom(GetOwner(), this);
            return true;
        }
        else
        {
            UE_LOG(LordCombat, Warning, TEXT("Failed to activate ability"));
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to activate ability"));
        }
    }

    return false;
}

bool UCombatComponent::HealUnit(UCombatComponent* TargetCombatComponent)
{
    bool bValid;
    FGameplayAbilitySpecHandle HealAbility = GetPreferredAbility(EAbilityTargetType::Heal, TargetCombatComponent->GetOwner(), bValid);
    if (bValid)
    {
        if (GetAbilitySubsystemComponent()->TryActivateAbility(HealAbility))
        {
            // Only refresh our combat time if the other unit is in combat still
            // We want to stay in combat as long as combat is relevant because healers
            // choose targets differently when in combat and when not.
            if (TargetCombatComponent->GetTimeSinceLastCombatAction() < 5)
            {
                MarkCombatTime();
            }
            BroadcastHeal(TargetCombatComponent->GetOwner(), TargetCombatComponent);
            //TargetCombatComponent->HandleAttackFrom(GetOwner(), this);
            return true;
        }
        else
        {
            UE_LOG(LordCombat, Warning, TEXT("Failed to activate ability"));
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to activate ability"));
        }
    }

    return false;
}

bool UCombatComponent::TrySelfBuff()
{
    bool bValid;
    FGameplayAbilitySpecHandle BuffAbility = GetPreferredAbility(EAbilityTargetType::Self, GetOwner(), bValid);
    if (bValid)
    {
        if (GetAbilitySubsystemComponent()->TryActivateAbility(BuffAbility))
        {
            BroadcastBuff(GetOwner(), this);
            return true;
        }
        else
        {
            UE_LOG(LordCombat, Warning, TEXT("Failed to activate ability"));
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to activate ability"));
        }
    }

    return false;
}

bool UCombatComponent::TryAllyBuff()
{
    bool bValid;
    FGameplayAbilitySpecHandle BuffAbility = GetPreferredAbility(EAbilityTargetType::Ally, GetOwner(), bValid);
    if (bValid)
    {
        if (GetAbilitySubsystemComponent()->TryActivateAbility(BuffAbility))
        {
            BroadcastBuff(GetOwner(), this);
            return true;
        }
        else
        {
            UE_LOG(LordCombat, Warning, TEXT("Failed to activate ability"));
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to activate ability"));
        }
    }

    return false;
}

FGameplayAbilitySpecHandle UCombatComponent::GetPreferredAttackAbility(const AActor* Target, bool& bFound) const
{
    return GetPreferredAbility(EAbilityTargetType::Enemy, Target, bFound);
}

FGameplayAbilitySpecHandle UCombatComponent::GetPreferredAbility_Implementation(EAbilityTargetType TargetType, const AActor* Target, bool& bFound) const
{
    // Should be SpecHandles, but GAS leaks the internal class here
    auto ASC = GetAbilitySubsystemComponent();
    FGameplayTagContainer TagContainer(UAbilityEnumsFunctionLibrary::GetTagForTargetType(TargetType));
    TArray<FGameplayAbilitySpec*> AvailableAbilities;

    bFound = false;

    ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(TagContainer, AvailableAbilities);

    // Ability list is not actually filtered to what can be activated (based on cost), so do that filtering now...
    AvailableAbilities.RemoveAll([ASC, Target](const FGameplayAbilitySpec* Spec) -> bool {
        // Note: This is largely copied from UAbilitySystemComponent::TryActivateAbility
        auto Ability = Spec->Ability;
        if (!Ability) return true;

        auto ActorInfo = ASC->AbilityActorInfo.Get();
        if (!ActorInfo || !ActorInfo->OwnerActor.IsValid() || !ActorInfo->AvatarActor.IsValid()) return true;

        if (!Ability->CanActivateAbility(Spec->Handle, ActorInfo))
        {
            return true; // remove
        }

        if (auto CombatAbility = Cast<UCombatAbility>(Ability))
        {
            if (!CombatAbility->CheckTargetValid(Target))
            {
                return true; // remove
            }
        }

        return false;
    });

    if (!AvailableAbilities.IsEmpty())
    {
        // Make copy of specs so that they can be passed into UFUNCTION.
        // Note this mimics what "UAbilitySystemComponent::TryActivateAbilitiesByTag" does.
        TArray<FGameplayAbilitySpec> AbilitiesCopy;
        AbilitiesCopy.Reserve(AvailableAbilities.Num());
        Algo::Transform(AvailableAbilities, AbilitiesCopy, [](FGameplayAbilitySpec* SpecPtr) { return *SpecPtr; });

        int Selected = PickPreferredAttackAbility(AbilitiesCopy, Target);
        if (Selected >= 0 && Selected < AvailableAbilities.Num())
        {
            bFound = true;
            return AvailableAbilities[Selected]->Handle;
        }
    }

    return FGameplayAbilitySpecHandle(); // Invalid handle
}

const int UCombatComponent::PickPreferredAttackAbility_Implementation(const TArray<FGameplayAbilitySpec>& AttackAbilities, const AActor* Target) const
{
    int MaxIndex = -1;
    int MaxPriority = MIN_int32;

    // Walk backwards to decide ties by thhe most recently-added ability
    for (int i = AttackAbilities.Num() - 1; i >= 0; i--)
    {
        const auto& Ability = AttackAbilities[i];
        int Priority = 0;
        if (const auto CombatAbility = Cast<UCombatAbility>(Ability.Ability))
        {
            Priority = CombatAbility->GetPriority();
        }
        if (Priority > MaxPriority)
        {
            MaxPriority = Priority;
            MaxIndex = i;
        }
    }

    return MaxIndex;
}

void UCombatComponent::SetTarget(UCombatComponent* InTarget)
{
    if (TargetComponent != InTarget)
    {
        TargetComponent = InTarget;
        OnTargetChange.Broadcast(TargetComponent);
    }
}

bool UCombatComponent::IsInvulnerable() const
{
    return GetAbilitySubsystemComponent()->HasMatchingGameplayTag(ULordGameplayTags::UnitStateInvulnerable());
}

void UCombatComponent::SetInvulnerable(bool bInvulnerable)
{
    // Implementing with gameplay tags to make some integrations (gameplay abilities) a little easier.
    auto ASC = GetAbilitySubsystemComponent();
    if (bInvulnerable && !IsInvulnerable())
    {
        auto* InvulnEffect = NewObject<UGEInvulnerability>(this, TEXT("Invulnerability Effect"));
        InvulnEffectHandle = ASC->ApplyGameplayEffectToSelf(InvulnEffect, 1, ASC->MakeEffectContext());
        OnInvulnerabilityChange.Broadcast(true);
    }
    else if (!bInvulnerable && IsInvulnerable())
    {
        if (ensureMsgf(InvulnEffectHandle.IsSet(), TEXT("Combat component cannot remove invulnerability, since it is set from something else")))
        {
            ensure(ASC->RemoveActiveGameplayEffect(InvulnEffectHandle.GetValue()));
            InvulnEffectHandle = NullOpt;
            OnInvulnerabilityChange.Broadcast(false);
        }
    }
}

bool UCombatComponent::IsTargetable() const
{
    // Generally untargetable?
    if (GetAbilitySubsystemComponent()->HasMatchingGameplayTag(ULordGameplayTags::UnitStateUntargetable()))
    {
        return false;
    }

    // In building is like removed from map
    if (GetAbilitySubsystemComponent()->HasMatchingGameplayTag(ULordGameplayTags::UnitStateVisiting()))
    {
        return false;
    }

    return true;
}

void UCombatComponent::MarkCombatTime()
{
    LastCombatTime = GetWorld()->GetTimeSeconds();
}

void UCombatComponent::ClearRecentCombatData()
{
    for (auto RevengeTarget : RecentRevengeTargets)
    {
        if (ensure(RevengeTarget)) // Why do these become null sometimes? The target gets nulled I think, but why isn't OnDeathLocal cleaning htem up before then?
        {
            RevengeTarget->OnDeathLocal.RemoveAll(this);
        }
    }
    RecentRevengeTargets.Empty();

    // Set time sentinel to denote there's no data
    LastCombatTime = 0;
}

void UCombatComponent::BroadcastDeath()
{
    OnDeathLocal.Broadcast();
    OnDeath.Broadcast();
    ReceiveOnDeath();
}

void UCombatComponent::BroadcastHealthChange()
{
    auto ASC = GetAbilitySubsystemComponent();
    auto AttributeSet = GetCombatAttributeSet();
    if (ensure(ASC && AttributeSet))
    {
        bool bIgnored;
        const int Health = (int) ASC->GetGameplayAttributeValue(AttributeSet->GetHealthAttribute(), bIgnored);
        const int MaxHealth = (int) ASC->GetGameplayAttributeValue(AttributeSet->GetMaxHealthAttribute(), bIgnored);
        OnHealthChange.Broadcast(Health, MaxHealth);
        ReceiveOnHealthChange(Health, MaxHealth);
    }
}

void UCombatComponent::BroadcastManaChange()
{
    auto ASC = GetAbilitySubsystemComponent();
    auto AttributeSet = GetCombatAttributeSet();
    if (ensure(ASC && AttributeSet))
    {
        bool bIgnored;
        const int Mana = (int)ASC->GetGameplayAttributeValue(AttributeSet->GetManaAttribute(), bIgnored);
        const int MaxMana = (int)ASC->GetGameplayAttributeValue(AttributeSet->GetMaxManaAttribute(), bIgnored);
        OnManaChange.Broadcast(Mana, MaxMana);
        ReceiveOnManaChange(Mana, MaxMana);
    }
}

void UCombatComponent::BroadcastAttack(AActor* Target, UCombatComponent* TargetCombatComponent)
{
    OnAttack.Broadcast(Target, TargetCombatComponent);
    ReceiveOnAttack(Target, TargetCombatComponent);
}

void UCombatComponent::BroadcastAttackLand(AActor* Target, UCombatComponent* TargetCombatComponent)
{
    OnAttackLand.Broadcast(Target, TargetCombatComponent);
    ReceiveOnAttackLand(Target, TargetCombatComponent);
}

void UCombatComponent::BroadcastAttackReceived(AActor* AttackingActor, UCombatComponent* AttackingCombatComponent)
{
    OnAttackReceived.Broadcast(AttackingActor, AttackingCombatComponent);
    ReceiveOnAttackReceived(AttackingActor, AttackingCombatComponent);
}

void UCombatComponent::BroadcastBuff(AActor* Target, UCombatComponent* TargetCombatComponent)
{
    OnBuff.Broadcast(Target, TargetCombatComponent);
}

void UCombatComponent::BroadcastHeal(AActor* Target, UCombatComponent* TargetCombatComponent)
{
    OnHeal.Broadcast(Target, TargetCombatComponent);
}

void UCombatComponent::OnOwnerPossessed(APawn* Pawn, AController* InOldController, AController* InNewController)
{
    auto OldController = Cast<AUnitController>(InOldController);
    auto NewController = Cast<AUnitController>(InNewController);
    if (OldController)
    {
        OldController->OnAITargetChange.RemoveAll(this);
    }
    if (NewController)
    {
        NewController->OnAITargetChange.AddUObject(this, &UCombatComponent::SetTarget);
    }
}

void UCombatComponent::AddRevengeTarget(UCombatComponent* RevengeTarget)
{
    if (RevengeTarget && !RecentRevengeTargets.Contains(RevengeTarget))
    {
        RecentRevengeTargets.Add(RevengeTarget);
        RevengeTarget->OnDeathLocal.AddWeakLambda(this, [this, RevengeTarget]()
        {
            RecentRevengeTargets.Remove(RevengeTarget);
            RevengeTarget->OnDeathLocal.RemoveAll(this);
        });
        MarkCombatTime();
    }
}

void UCombatComponent::HandleAttackFrom(AActor* AttackingActor, UCombatComponent* AttackingCombatComponent)
{
    AddRevengeTarget(AttackingCombatComponent);
    BroadcastAttackReceived(AttackingActor, AttackingCombatComponent);
}

bool UCombatComponent::IsTargetableEnemy(const UCombatComponent* Other, bool bRequireAlive) const
{
    return (!bRequireAlive || !Other->IsDead())
        && (GetTeam() != Other->GetTeam())
        && (Other->IsTargetable())
        ;
}

UCombatComponent* UCombatComponent::GetNearestEnemy(bool bAlive) const
{
    auto Owner = GetOwner();
    auto ASC = GetAbilitySubsystemComponent();
    auto AttributeSet = GetCombatAttributeSet();
    if (!ensure(Owner) || !ensure(ASC) || !ensure(AttributeSet))
    {
        return nullptr;
    }

    bool bIgnored;
    float Sight = ASC->GetGameplayAttributeValue(AttributeSet->GetSightAttribute(), bIgnored);

    return UGameplayUtils::GetNearestCombatComponentNearLocationEx(GetWorld(), GetOwner()->GetActorLocation(), Sight,
        [this, bAlive](const UCombatComponent* Other) -> bool {
            return !IsTargetableEnemy(Other, bAlive);
        }
        );
}

TArray<UCombatComponent*> UCombatComponent::GetNearbyEnemies(float Radius, bool bAlive) const
{
    auto Owner = GetOwner();
    auto ASC = GetAbilitySubsystemComponent();
    auto AttributeSet = GetCombatAttributeSet();
    if (!ensure(Owner) || !ensure(ASC) || !ensure(AttributeSet))
    {
        return {};
    }

    if (Radius <= 0)
    {
        bool bIgnored;
        Radius = ASC->GetGameplayAttributeValue(AttributeSet->GetSightAttribute(), bIgnored);
    }

    auto Nearby = UGameplayUtils::GetCombatComponentsNearLocation(GetWorld(), GetOwner()->GetActorLocation(), Radius);
    Nearby.RemoveAll([this, bAlive](const UCombatComponent* Other) -> bool { return !IsTargetableEnemy(Other, bAlive); });
    return Nearby;

}

TArray<UCombatComponent*> UCombatComponent::GetNearbyAllies(float Radius, bool bAlive) const
{
    auto Owner = GetOwner();
    auto ASC = GetAbilitySubsystemComponent();
    auto AttributeSet = GetCombatAttributeSet();
    if (!ensure(Owner) || !ensure(ASC) || !ensure(AttributeSet))
    {
        return {};
    }

    if (Radius <= 0)
    {
        bool bIgnored;
        Radius = ASC->GetGameplayAttributeValue(AttributeSet->GetSightAttribute(), bIgnored);
    }

    auto Nearby = UGameplayUtils::GetCombatComponentsNearLocation(GetWorld(), GetOwner()->GetActorLocation(), Radius);
    Nearby.RemoveAll([this, bAlive](const UCombatComponent* Other) -> bool
        {
            return (bAlive && Other->IsDead())
                || Other->GetTeam() != GetTeam()
                ;
        });
    return Nearby;

}

TArray<UCombatComponent*> UCombatComponent::GetNearbyHurtAllies(float Radius) const
{
    auto Allies = GetNearbyAllies(Radius, true);
    Allies.RemoveAll([](const UCombatComponent* Ally) -> bool { return Ally->GetHealth() >= Ally->GetMaxHealth(); });
    return Allies;
}

TArray<UCombatComponent*> UCombatComponent::GetRecentAttackers() const
{
    return RecentRevengeTargets;
}

TArray<FActiveGameplayEffectHandle> UCombatComponent::GetActiveEffectHandles() const
{
    auto ASC = GetAbilitySubsystemComponent();
    if (ensure(ASC))
    {
        FGameplayEffectQuery Query;
        return ASC->GetActiveEffects(Query);
    }

    return {};
}

TArray<const UGameplayEffect*> UCombatComponent::GetActiveEffects() const
{
    TArray<const UGameplayEffect*> Effects;
    auto ASC = GetAbilitySubsystemComponent();
    for (auto Handle : GetActiveEffectHandles())
    {
        Effects.Add(ASC->GetGameplayEffectDefForHandle(Handle));
    }

    return Effects;
}

TArray<const UVisibleGameplayEffect*> UCombatComponent::GetActiveVisibleEffects() const
{
    TArray<const UVisibleGameplayEffect*> VisibleEffects;
    auto Effects = GetActiveEffects();
    VisibleEffects.Reserve(VisibleEffects.Num());
    for (auto Effect : Effects)
    {
        if (auto VisibleEffect = Cast<UVisibleGameplayEffect>(Effect))
        {
            VisibleEffects.Add(VisibleEffect);
        }
    }

    return VisibleEffects;
}

void UCombatComponent::HandleLevelChange(int NewLevel)
{
    if (auto Abilities = LevelUpAbilities.Find(NewLevel))
    {
        for (auto& Ability : Abilities->Array)
        {
            GiveCombatAbility(Ability);
        }
    }
}

/*static*/ UCombatComponent* UCombatComponent::GetComponentForActor(AActor* Actor)
{
    if (!IsValid(Actor))
    {
        return nullptr;
    }

    return Actor->GetComponentByClass<UCombatComponent>();
}
