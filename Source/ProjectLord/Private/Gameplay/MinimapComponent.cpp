// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/MinimapComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/Map.h"
#include "UI/ViewModels/MinimapIconViewModel.h"

UMinimapComponent::UMinimapComponent()
{
	bRegistered = false;
	bAutoRegister = true;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = .25;
}

void UMinimapComponent::BeginPlay()
{
	Super::BeginPlay();

	ViewModel = CreateLordVM<UVMMinimapIcon>(this);
	ViewModel->SetTeam(GetTeam());
	ViewModel->SetIcon(GetMapIcon());
	ViewModel->SetWorldLocation(GetWorldLocation());
	ViewModel->SetVisible(/*IsVisible()*/ false); // Start not visible, to try and avoid a visible blip of units in FoW

	if (bAutoRegister)
	{
		RegisterWithMinimap();
	}
}

void UMinimapComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
	UnregisterFromMinimap(); // Handles if already unregistred
}

void UMinimapComponent::RegisterWithMinimap()
{
	if (!ensure(!bRegistered))
	{
		return;
	}

	auto Map = AMap::GetMap(this);
	if (ensure(Map))
	{
		bRegistered = true;
		Map->AddMapComponent(this);
	}
}

void UMinimapComponent::UnregisterFromMinimap()
{
	if (bRegistered)
	{
		auto Map = AMap::GetMap(this);
		if (Map) // No ensure, as map may also have been removed on level teardown
		{
			Map->RemoveMapComponent(this);
			bRegistered = false;
		}
	}
}

bool UMinimapComponent::IsVisible() const
{
	return !GetOwner()->IsHidden();
}

FVector UMinimapComponent::GetWorldLocation() const
{
	return GetOwner()->GetActorLocation();
}

void UMinimapComponent::SetTeam(EGameTeam InTeam)
{
	Team = InTeam;
	ViewModel->SetTeam(GetTeam());
}

void UMinimapComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto WorldLocation = GetWorldLocation();
	ViewModel->SetWorldLocation(WorldLocation);
	ViewModel->SetVisible(IsVisible());
}
