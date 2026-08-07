// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/Creature.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "Gameplay/GameplayUtils.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Attributes/CreatureAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Buildings/ResidentialBuilding.h"
#include "Gameplay/Units/HeroBase.h"

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

void ACreature::SetHomeBuilding(AResidentialBuilding* Building)
{
    HomeBuilding = Building;
}

void ACreature::OnDeath_Implementation()
{
    // Super wants to be overriden complete.
    //Super::OnDeath_Implementation();
    

    // TODO #46 Graves
    // This should all happen when a grave expires. Hero counts linger, for example.
    {
        if (HomeBuilding.IsValid())
        {
            HomeBuilding.Get()->RemoveResident(this);
        }
        this->Destroy();
    }
}

bool ACreature::AwardGoldToNearbyHeroes(int Gold)
{
    constexpr float Radius = 200;
    auto NearbyActors = UGameplayUtils::GetActorsNear(this, Radius);
    TArray<AHeroBase*> Heroes;
    for (auto Actor : NearbyActors)
    {
        if (AHeroBase* Hero = Cast<AHeroBase>(Actor))
        {
            Heroes.Add(Hero);
        }
    }

    if (Heroes.IsEmpty())
    {
        return false;
    }

    int GoldEach = FMath::Max(1, FMath::CeilToInt((float) Gold / (float)Heroes.Num()));
    for (auto Hero : Heroes)
    {
        Hero->AwardGold(GoldEach);
    }
    return true;
}
