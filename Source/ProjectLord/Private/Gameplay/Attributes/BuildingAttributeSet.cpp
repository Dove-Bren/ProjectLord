// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Attributes/BuildingAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UBuildingAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, GoldGeneration);

}
