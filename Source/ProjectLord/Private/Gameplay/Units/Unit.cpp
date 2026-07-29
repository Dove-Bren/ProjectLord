// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/Unit.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "LordLogging.h"
#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/AI/UnitController.h"
#include "Gameplay/Attributes/UnitBaseAttributes.h"
#include "Gameplay/Combat/CombatComponent.h"
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

    SelectionComponent = CreateDefaultSubobject<USelectionComponent>(TEXT("Selection"));
    SelectionComponent->SetSelectable(true);

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
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
    return CombatComponent->IsDead();
}

void AUnit::OnDeath_Implementation()
{
    UE_LOG(LordUnit, Warning, TEXT("Unit did not override OnDeath!"));
    this->Destroy();
}

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
                    UE_LOG(LordUnit, Error, TEXT("Unit [%s]'s base attribute specifies a value for [%s]%s, but unit does not have that attribute"),
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
