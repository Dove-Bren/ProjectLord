// Copyright (c) Skyler Manzanares. All Rights Reserved.

#include "Gameplay/Units/Unit.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/Combat/Ability/LordUnitAttributeSet.h"
#include "Gameplay/Combat/Ability/LordHeroAttributeSet.h"
#include "Gameplay/Units/AI/UnitController.h"

AUnit::AUnit() : ACharacter()
{
    // Set up defaults
    WanderRadius = 1000;
    Sight = 1000;
    AttackRange = 100;

    Level = 1;
    Team = EUnitTeam::Monster;

    // Adjust character stuff
    GetCapsuleComponent()->InitCapsuleSize(22.0f, 50.0f);
    GetCharacterMovement()->bUseControllerDesiredRotation = true;

    // GAS
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySubsystem"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full); // I think full, because we want to see things everywhere?

    LordUnitAttributeSet = CreateDefaultSubobject<ULordUnitAttributeSet>(TEXT("LordUnitAttributeSet"));
    LordHeroAttributeSet = CreateDefaultSubobject<ULordHeroAttributeSet>(TEXT("LordHeroAttributeSet"));
}

void AUnit::BeginPlay()
{
    Super::BeginPlay();

    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        for (auto& Ability : DefaultAbilities)
        {
            AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
        }

        // TODO: effects (for base stats)

        // Make sure to prompt attribute set to recalc depending attributes
        LordHeroAttributeSet->Init(LordUnitAttributeSet, true);

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(LordUnitAttributeSet->GetHealthAttribute())
            .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
                {
                    if (IsAlive() && ChangeData.NewValue <= 0)
                    {
                        OnDeath();
                    }
                });
    }
}

void AUnit::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(LordUnitAttributeSet->GetHealthAttribute())
            .RemoveAll(this);
    }
}

AUnitController* AUnit::GetUnitController() const
{
    return Cast<AUnitController>(GetController());
}

bool AUnit::IsCloseEnoughToAttack(const AUnit* OtherUnit) const
{
    return this->GetDistanceTo(OtherUnit) <= AttackRange;
}

bool AUnit::CanAttack_Implementation() const
{
    return !AbilitySystemComponent->HasMatchingGameplayTag(ULordGameplayTags::UnitStateAttacking());
}

void AUnit::DamageUnit_Implementation(FAttackDamage InDamage)
{
    if (IsAlive())
    {
        const int Defense = GetDefenseFor(InDamage.DamageType);
        const int TotalDamage = FMath::Clamp(InDamage.Amount - Defense, 0, MaxHealth);
        Health = FMath::Clamp(Health - TotalDamage, 0, MaxHealth); // TODO setter?

        if (IsDead())
        {
            OnDeath();
        }
    }
}

void AUnit::AttackUnit_Implementation(AUnit* TargetUnit)
{
    FGameplayAbilitySpecHandle AttackAbility = GetPreferredAttackAbility();
    if (AttackAbility.IsValid())
    {
        if (!AbilitySystemComponent->TryActivateAbility(AttackAbility))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to activate ability"));
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to activate ability"));
        }
    }
}

void AUnit::OnDeath_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Unit did not override OnDeath!"));
    this->Destroy();
}

FDefenses AUnit::GetDefenses_Implementation() const
{
    return BaseDefense;
}

FGameplayAbilitySpecHandle AUnit::GetPreferredAttackAbility_Implementation() const
{
    // Should be SpecHandles, but GAS leaks the internal class here
    FGameplayTagContainer TagContainer(ULordGameplayTags::AbilityTypeAttack());
    TArray<FGameplayAbilitySpec*> AvailableAbilities;
    AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(TagContainer, AvailableAbilities);

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

const int AUnit::PickPreferredAttackAbility_Implementation(const TArray<FGameplayAbilitySpec>& AttackAbilities) const
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
