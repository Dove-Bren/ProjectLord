// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/Creature.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Attributes/CreatureAttributeSet.h"
#include "Gameplay/Attributes/UnitBaseAttributes.h"

#define MOVEMENT_STAT_TO_UE_SPEED(InMovement) (InMovement * 50)

ACreature::ACreature()
{
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

void ACreature::BeginPlay()
{
    Super::BeginPlay();
}

void ACreature::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CreatureAttributeSet->GetMovementAttribute())
            .RemoveAll(this);
    }
}
