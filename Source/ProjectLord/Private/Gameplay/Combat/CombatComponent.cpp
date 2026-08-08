// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/CombatComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

#include "LordLogging.h"
#include "Gameplay/GameplayUtils.h"
#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/AI/UnitController.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Buildings/Building.h"
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

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetCombatAttributeSet()->GetHealthAttribute())
            .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
                {
                    if (ChangeData.OldValue > 0 && ChangeData.NewValue <= 0)
                    {
                        BroadcastDeath();
                    }
                });
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
    if (LastCombatTime != 0
        && GetTimeSinceLastCombatAction() > CombatTimeoutSeconds)
    {
        ClearRecentCombatData();
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

TArray<UCombatAbility*> UCombatComponent::GetCombatAbilities(bool bIncludeHidden)
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

bool UCombatComponent::IsDead() const
{
    bool bIgnored;
    int Health = FMath::TruncToInt(GetAbilitySubsystemComponent()->GetGameplayAttributeValue(GetCombatAttributeSet()->GetHealthAttribute(), bIgnored));
    return Health <= 0;
}

bool UCombatComponent::IsCloseEnoughToAttack(const UCombatComponent* OtherCombatComponent) const
{
    bool bIgnored;
    auto Owner = GetOwner();
    auto Target = OtherCombatComponent->GetOwner();
    const auto AttackRange = GetAbilitySubsystemComponent()->GetGameplayAttributeValue(GetCombatAttributeSet()->GetAttackRangeAttribute(), bIgnored);
    const auto Dist = Owner->GetDistanceTo(Target);

    // Things have different sized hitboxes. Buildings for example are large, and
    // it might not be feasible to get within X units of the _center_ of it.
    // So check distance to the outer edge of the target's size, roughly;
    FVector OwnerOrigin, OwnerBounds;
    FVector TargetOrigin, TargetBounds;

    Owner->GetActorBounds(true, OwnerOrigin, OwnerBounds);
    Target->GetActorBounds(true, TargetOrigin, TargetBounds);

    // Note: not sure if square, or if x/y would be bigger. So average.
    // Maybe should take max?
    const double OwnerHalfWidth = (OwnerBounds.X + OwnerBounds.Y) / 2.0f;
    const double TargetHalfWidth = (TargetBounds.X + TargetBounds.Y) / 2.0f;

    return Dist <= (AttackRange + OwnerHalfWidth + TargetHalfWidth);
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
    FGameplayAbilitySpecHandle AttackAbility = GetPreferredAttackAbility();
    if (AttackAbility.IsValid())
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

FGameplayAbilitySpecHandle UCombatComponent::GetPreferredAttackAbility_Implementation() const
{
    // Should be SpecHandles, but GAS leaks the internal class here
    FGameplayTagContainer TagContainer(ULordGameplayTags::AbilityTypeAttack());
    TArray<FGameplayAbilitySpec*> AvailableAbilities;
    GetAbilitySubsystemComponent()->GetActivatableGameplayAbilitySpecsByAllMatchingTags(TagContainer, AvailableAbilities);

    if (!AvailableAbilities.IsEmpty())
    {
        // Make copy of specs so that they can be passed into UFUNCTION.
        // Note this mimics what "UAbilitySystemComponent::TryActivateAbilitiesByTag" does.
        TArray<FGameplayAbilitySpec> AbilitiesCopy;
        AbilitiesCopy.Reserve(AvailableAbilities.Num());
        Algo::Transform(AvailableAbilities, AbilitiesCopy, [](FGameplayAbilitySpec* SpecPtr) { return *SpecPtr; });

        int Selected = PickPreferredAttackAbility(AbilitiesCopy);
        if (Selected >= 0 && Selected < AvailableAbilities.Num())
        {
            return AvailableAbilities[Selected]->Handle;
        }
    }

    return FGameplayAbilitySpecHandle(); // Invalid handle
}

const int UCombatComponent::PickPreferredAttackAbility_Implementation(const TArray<FGameplayAbilitySpec>& AttackAbilities) const
{
    int MaxIndex = -1;
    int MaxLevel = MIN_int32;
    for (int i = 0; i < AttackAbilities.Num(); i++)
    {
        const auto& Ability = AttackAbilities[i];
        if (Ability.Level > MaxLevel)
        {
            MaxLevel = Ability.Level;
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
        UGameplayEffect* InvulnEffect = NewObject<UGameplayEffect>(this, TEXT("Invulnerability Effect"));
        InvulnEffect->DurationPolicy = EGameplayEffectDurationType::Infinite;
        // This feels like a LOT too apply a gameplay tag...
        {
            auto& TagsComp = InvulnEffect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
            FInheritedTagContainer AppliedTags = TagsComp.GetConfiguredTargetTagChanges(); // Copy what's there
            AppliedTags.AddTag(ULordGameplayTags::UnitStateInvulnerable());
            TagsComp.SetAndApplyTargetTagChanges(AppliedTags);
        }
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

void UCombatComponent::MarkCombatTime()
{
    LastCombatTime = GetWorld()->GetTimeSeconds();
}

void UCombatComponent::ClearRecentCombatData()
{
    RecentRevengeTargets.Empty();

    // Set time sentinel to denote there's no data
    LastCombatTime = 0;
}

void UCombatComponent::BroadcastDeath()
{
    OnDeath.Broadcast();
    ReceiveOnDeath();
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
    RecentRevengeTargets.AddUnique(RevengeTarget);
    MarkCombatTime();
}

void UCombatComponent::HandleAttackFrom(AActor* AttackingActor, UCombatComponent* AttackingCombatComponent)
{
    AddRevengeTarget(AttackingCombatComponent);
    BroadcastAttackReceived(AttackingActor, AttackingCombatComponent);
}

UCombatComponent* UCombatComponent::GetNearestEnemy(bool bAlive)
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
            return (bAlive && Other->IsDead())
                || (GetTeam() == Other->GetTeam())
                ;
        }
        );
}

TArray<UCombatComponent*> UCombatComponent::GetRecentAttackers() const
{
    return RecentRevengeTargets;
}

/*static*/ UCombatComponent* UCombatComponent::GetComponentForActor(AActor* Actor)
{
    if (!IsValid(Actor))
    {
        return nullptr;
    }

    return Actor->GetComponentByClass<UCombatComponent>();
}
