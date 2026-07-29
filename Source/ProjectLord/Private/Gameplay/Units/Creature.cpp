// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/Creature.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/CreatureAttributeSet.h"
#include "Gameplay/Units/UnitBaseAttributes.h"

#define MOVEMENT_STAT_TO_UE_SPEED(InMovement) (InMovement * 50)

ACreature::ACreature()
{
    CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));
	CreatureAttributeSet = CreateDefaultSubobject<UCreatureAttributeSet>(TEXT("CreatureAttributeSet"));
}

void ACreature::RegisterAttributes()
{
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CreatureAttributeSet->GetMovementAttribute())
        .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
            {
                GetCharacterMovement()->MaxWalkSpeed = MOVEMENT_STAT_TO_UE_SPEED(ChangeData.NewValue);
            });
}

void ACreature::SetupBaseAttributes()
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

void ACreature::BeginPlay()
{
    Super::BeginPlay();

    if (ensure(AbilitySystemComponent))
    {
        //MOVE HERE AbilitySystemComponent->InitAbilityActorInfo(this, this);
        RegisterAttributes();
        SetupBaseAttributes();
    }

    CombatComponent->OnDeath.AddDynamic(this, &ACreature::HandleDeath);
}

void ACreature::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CreatureAttributeSet->GetMovementAttribute())
            .RemoveAll(this);
    }
    if (IsValid(CombatComponent))
    {
        CombatComponent->OnDeath.RemoveAll(this);
    }
}

void ACreature::HandleDeath()
{
    // Rebroadcast
    OnDeath();
}