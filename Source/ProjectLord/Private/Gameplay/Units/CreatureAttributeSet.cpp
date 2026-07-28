// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/CreatureAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UCreatureAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCreatureAttributeSet, Movement);
	DOREPLIFETIME(UCreatureAttributeSet, WanderRadius);
}
