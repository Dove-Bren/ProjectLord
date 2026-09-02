// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/FogOfWar.h"

#include "Gameplay/FogOfWarSubsystem.h"

void AFogOfWar::BeginPlay()
{
	Super::BeginPlay();

	auto Subsystem = GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
	if (ensure(Subsystem))
	{
		Subsystem->Activate(this);
	}
}
