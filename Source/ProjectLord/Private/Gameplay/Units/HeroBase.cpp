// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroBase.h"

#include "Gameplay/Combat/Ability/LordUnitAttributeSet.h"
#include "Gameplay/Combat/Ability/LordHeroAttributeSet.h"

AHeroBase::AHeroBase() : ACreature()
{
	Team = EUnitTeam::Player1;
	Gold = 0;
	HeroXP = 0;

	LordHeroAttributeSet = CreateDefaultSubobject<ULordHeroAttributeSet>(TEXT("LordHeroAttributeSet"));
}

int AHeroBase::GetHeroMaxXP() const
{
	bool bIgnored;
	const int Level = FMath::Clamp((int) AbilitySystemComponent->GetGameplayAttributeValue(LordUnitAttributeSet->GetLevelAttribute(), bIgnored), 1, 9999);
	return 100 + Level * 20;
}

void AHeroBase::BeginPlay()
{
	Super::BeginPlay();
}

void AHeroBase::SetupBaseAttributes()
{
	// Make sure to set up association first
	LordHeroAttributeSet->Init(LordUnitAttributeSet);

	// Let base class take care of setting up base values
	Super::SetupBaseAttributes();

	// Make sure to prompt attribute set to recalc dependent attributes
	LordHeroAttributeSet->UpdateDerivedUnitValues();
}
