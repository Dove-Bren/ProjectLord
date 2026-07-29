// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/Unit.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/Attributes/UnitBaseAttributes.h"
#include "Gameplay/Combat/Ability/UnitAbility.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/AI/UnitController.h"
#include "UI/ViewModels/Units/UnitViewModel.h"

AUnit::AUnit() : ACharacter()
{
    // Set up defaults
    Team = EUnitTeam::Monster;

    // Adjust character stuff
    GetCapsuleComponent()->InitCapsuleSize(22.0f, 50.0f);
    GetCharacterMovement()->bUseControllerDesiredRotation = true;

    // GAS
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySubsystem"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full); // I think full, because we want to see things everywhere?

    CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

TArray<UUnitAbility*> AUnit::GetUnitAbilities(bool bIncludeHidden)
{
    return CombatComponent->GetCombatAbilities(bIncludeHidden);
    /*TArray<UUnitAbility*> Abilities;

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

    return Abilities;*/
}

void AUnit::FaceActor(AActor* OtherActor)
{
    // Face target
    auto Diff = OtherActor->GetActorLocation() - GetActorLocation();
    Diff.Z = 0;
    SetActorRotation(Diff.Rotation());
}

void AUnit::BeginPlay()
{
    Super::BeginPlay();

    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }

    RegisterAttributes();
    SetupBaseAttributes();

    CombatComponent->OnDeath.AddDynamic(this, &AUnit::HandleDeath);
    CombatComponent->OnAttack.AddDynamic(this, &AUnit::HandleAttack);
}

void AUnit::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    if (IsValid(CombatComponent))
    {
        CombatComponent->OnDeath.RemoveAll(this);
    }
}

AUnitController* AUnit::GetUnitController() const
{
    return Cast<AUnitController>(GetController());
}

bool AUnit::IsDead() const
{
    /*bool bIgnored;
    int Health = FMath::TruncToInt(AbilitySystemComponent->GetGameplayAttributeValue(LordUnitAttributeSet->GetHealthAttribute(), bIgnored));
    return Health <= 0;*/
    return CombatComponent->IsDead();
}

bool AUnit::IsCloseEnoughToAttack(const AUnit* OtherUnit) const
{
    /*bool bIgnored;
    return this->GetDistanceTo(OtherUnit) <= AbilitySystemComponent->GetGameplayAttributeValue(LordUnitAttributeSet->GetAttackRangeAttribute(), bIgnored);*/
    return CombatComponent->IsCloseEnoughToAttack(OtherUnit->CombatComponent);
}

int AUnit::GetDefenseFor(EDamageType InType) const
{
    /*FGameplayAttribute Attribute;
    switch (InType)
    {
    case EDamageType::Melee:
        Attribute = LordUnitAttributeSet->GetMeleeDefenseAttribute();
        break;
    case EDamageType::Ranged:
        Attribute = LordUnitAttributeSet->GetRangedDefenseAttribute();
        break;
    case EDamageType::Magic:
        Attribute = LordUnitAttributeSet->GetMagicDefenseAttribute();
        break;
    default:
        return 0;
    }
    bool bIgnored;
    return FMath::TruncToInt(AbilitySystemComponent->GetGameplayAttributeValue(Attribute, bIgnored));*/
    return CombatComponent->GetDefenseFor(InType);
}

bool AUnit::CanAttack_Implementation() const
{
    //return !AbilitySystemComponent->HasMatchingGameplayTag(ULordGameplayTags::UnitStateAttacking());
    return CombatComponent->CanAttack();
}

void AUnit::AttackUnit_Implementation(AUnit* TargetUnit)
{
    auto TargetComponent = TargetUnit->CombatComponent;
    if (CombatComponent->AttackUnit(TargetComponent))
    {
        FaceActor(TargetUnit);
    }
}

void AUnit::OnDeath_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Unit did not override OnDeath!"));
    this->Destroy();
}

FGameplayAbilitySpecHandle AUnit::GetPreferredAttackAbility_Implementation() const
{
    //// Should be SpecHandles, but GAS leaks the internal class here
    //FGameplayTagContainer TagContainer(ULordGameplayTags::AbilityTypeAttack());
    //TArray<FGameplayAbilitySpec*> AvailableAbilities;
    //AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(TagContainer, AvailableAbilities);

    //if (!AvailableAbilities.IsEmpty())
    //{
    //    // Make copy of specs so that they can be passed into UFUNCTION.
    //    // Note this mimics what "UAbilitySystemComponent::TryActivateAbilitiesByTag" does.
    //    TArray<FGameplayAbilitySpec> AbilitiesCopy;
    //    AbilitiesCopy.Reserve(AvailableAbilities.Num());
    //    Algo::Transform(AvailableAbilities, AbilitiesCopy, [](FGameplayAbilitySpec* SpecPtr) { return *SpecPtr; });

    //    int Selected = PickPreferredAttackAbility(AbilitiesCopy);
    //    if (Selected >= 0 && Selected < AvailableAbilities.Num())
    //    {
    //        return AvailableAbilities[Selected]->Handle;
    //    }
    //}

    //return FGameplayAbilitySpecHandle(); // Invalid handle
    return CombatComponent->GetPreferredAttackAbility();
}

//const int AUnit::PickPreferredAttackAbility_Implementation(const TArray<FGameplayAbilitySpec>& AttackAbilities) const
//{
//    int MaxIndex = -1;
//    int MaxLevel = MIN_int32;
//    for (int i = 0; i < AttackAbilities.Num(); i++)
//    {
//        const auto& Ability = AttackAbilities[i];
//        if (Ability.Level > MaxLevel)
//        {
//            MaxLevel = Ability.Level;
//            MaxIndex = i;
//        }
//    }
//
//    return MaxIndex;
//}

void AUnit::RegisterAttributes()
{
    ;
}

void AUnit::SetupBaseAttributes()
{
    if (IsValid(ClassAttributeDefaults))
    {
        FString Context = TEXT("DefaultUnitAttributeIter");
        ClassAttributeDefaults->ForeachRow<FUnitBaseAttributes>(Context, [this](const FName& Key, const FUnitBaseAttributes& Value)
            {
                if (!AbilitySystemComponent->HasAttributeSetForAttribute(Value.Attribute))
                {
                    UE_LOG(LogTemp, Error, TEXT("Unit [%s]'s base attribute specifies a value for [%s]%s, but unit does not have that attribute"),
                        *GetDebugName(this),
                        *Key.ToString(),
                        *Value.Attribute.AttributeName
                    );
                    return;
                }

                double AttributeValue = Value.BaseValue;
                if (Value.Variation > 0)
                {
                    const int Variation = FMath::FloorToInt(Value.Variation);
                    AttributeValue += FMath::RandRange(-Variation, Variation);
                }

                AbilitySystemComponent->SetNumericAttributeBase(Value.Attribute, AttributeValue);

            });
    }
}

void AUnit::HandleDeath()
{
    // Rebroadcast
    OnDeath();
}

void AUnit::HandleAttack(AActor* Target, UCombatComponent* TargetComponent)
{
    FaceActor(Target);
}

UVMUnit* AUnit::GetUnitVM()
{
    if (UnitVM)
    {
        return UnitVM;
    }

    UnitVM = UVMUnit::CreateForUnit(this);
    UnitVM->InitializeAttributeListeners(AbilitySystemComponent, CombatAttributeSet);

    // Note: For now, team is only ever set on construction of the AUnit.
    UnitVM->SetUnitTeam(Team);

    return UnitVM;
}
