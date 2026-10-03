// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Map/FogOfWar.h"

#include "Gameplay/Map/FogOfWarSubsystem.h"

void AFogOfWar::BeginPlay()
{
	Super::BeginPlay();

	auto Subsystem = GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
	if (ensure(Subsystem))
	{
		Subsystem->Activate(this);
	}
}
