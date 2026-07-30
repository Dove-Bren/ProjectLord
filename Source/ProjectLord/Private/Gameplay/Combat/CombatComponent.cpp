// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/CombatComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#include "LordLogging.h"
#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/AI/UnitController.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Units/Unit.h"

UCombatComponent::UCombatComponent()
{
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
}

void UCombatComponent::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);
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

UCombatComponent* UCombatComponent::GetCombatTarget() const
{
    auto Owner = GetOwner();
    if (auto Unit = Cast<AUnit>(Owner))
    {
        auto UnitController = Unit->GetUnitController();
        if (ensure(UnitController))
        {
            return UnitController->GetTargetComponent();
        }
    }
    if (auto Building = Cast<ABuilding>(Owner))
    {
        // TODO
    }

    ensureMsgf(false, TEXT("Owning Actor (%s) is neither a Unit or a building"), *GetNameSafe(Owner));
    return nullptr;
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

TArray<UUnitAbility*> UCombatComponent::GetCombatAbilities(bool bIncludeHidden)
{
    TArray<UUnitAbility*> Abilities;

    auto AbilitySystemComponent = GetAbilitySubsystemComponent();
    if (ensure(AbilitySystemComponent))
    {
        TArray<FGameplayAbilitySpecHandle> AllAbilities;
        AbilitySystemComponent->GetAllAbilities(AllAbilities);
        for (const auto& Handle : AllAbilities)
        {
            auto Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
            if (auto Ability = Cast<UUnitAbility>(Spec->Ability))
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
    return GetOwner()->GetDistanceTo(OtherCombatComponent->GetOwner()) <= GetAbilitySubsystemComponent()->GetGameplayAttributeValue(GetCombatAttributeSet()->GetAttackRangeAttribute(), bIgnored);
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
            BroadcastAttack(TargetCombatComponent->GetOwner(), TargetCombatComponent);
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
