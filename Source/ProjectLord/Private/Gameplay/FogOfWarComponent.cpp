// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/FogOfWarComponent.h"

#include "Gameplay/FogOfWarSubsystem.h"

/*static*/ UFogOfWarSubsystem* UFogOfWarComponent::GetSubsystem(const UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
}

UFogOfWarComponent::UFogOfWarComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bRegistered = false;
	bAutoRegister = true;
}

void UFogOfWarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoRegister)
	{
		RegisterWithSubsystem();
	}
}

void UFogOfWarComponent::RegisterWithSubsystem()
{
	if (!ensure(!bRegistered))
	{
		return;
	}

	auto Subsystem = GetSubsystem(this);
	if (ensure(Subsystem))
	{
		bRegistered = true;
		Subsystem->AddRevealer(this);
	}
}

void UFogOfWarComponent::UnregisterFromSubsystem()
{
	if (bRegistered)
	{
		auto Subsystem = GetSubsystem(this);
		if (ensure(Subsystem))
		{
			Subsystem->RemoveRevealer(this);
			bRegistered = false;
		}
	}
}

void UFogOfWarComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
	UnregisterFromSubsystem(); // Handles if already unregistred
}

FVector UFogOfWarComponent::GetRevealLocation() const
{
	return GetOwner()->GetActorLocation();
}

