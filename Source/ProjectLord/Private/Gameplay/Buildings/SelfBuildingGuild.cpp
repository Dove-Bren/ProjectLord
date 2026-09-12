// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/SelfBuildingGuild.h"

#include "Gameplay/Combat/GameplayEffect/SelfRepairGameplayEffect.h"

void ASelfBuildingGuild::BeginPlay()
{
	Super::BeginPlay();

	NextRepairTime = RepairPeriod;
	RepairEffectHandle = AbilitySystemComponent->MakeOutgoingSpec(UGESelfRepair::StaticClass(), 1, AbilitySystemComponent->MakeEffectContext());
}

void ASelfBuildingGuild::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Repair self
	if (!AtFullHealth())
	{
		auto Now = GetGameTimeSinceCreation();
		while (TimeToRepair(Now))
		{
			NextRepairTime += RepairPeriod; // Add instead of setting from now, so time dilation can work
			DoAutoRepair();
		}
	}
}

bool ASelfBuildingGuild::TimeToRepair(double Now) const
{
	return Now >= NextRepairTime;
}

void ASelfBuildingGuild::DoAutoRepair()
{
	if (ensure(RepairEffectHandle.IsValid()))
	{
		// Make a copy
		auto RepairEffectSpec = *RepairEffectHandle.Data.Get();
		RepairEffectSpec.SetSetByCallerMagnitude(UGESelfRepair::AmountName, HealthPerRepair);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(RepairEffectSpec);

		if (AtFullHealth())
		{
			NotifyRepairComplete();
		}
		else
		{
			NotifyRepairAction();
		}
	}
}