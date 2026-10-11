// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Map/MinimapComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Map/Map.h"
#include "UI/ViewModels/MinimapIconViewModel.h"

UMinimapComponent::UMinimapComponent()
{
	bRegistered = false;
	bAutoRegister = true;
	bOnlyVisibleToTeam = false;

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
		Map->AddMinimapComponent(this);
	}
}

void UMinimapComponent::UnregisterFromMinimap()
{
	if (bRegistered)
	{
		auto Map = AMap::GetMap(this);
		if (Map) // No ensure, as map may also have been removed on level teardown
		{
			Map->RemoveMinimapComponent(this);
			bRegistered = false;
		}
	}
}

bool UMinimapComponent::IsVisible(EGameTeam TeamToCheck) const
{
	return !GetOwner()->IsHidden()
		&& (!bOnlyVisibleToTeam || TeamToCheck == GetTeam());
}

FVector UMinimapComponent::GetWorldLocation() const
{
	return GetOwner()->GetActorLocation();
}

void UMinimapComponent::SetTeam(EGameTeam InTeam)
{
	Team = InTeam;
	if (ViewModel)
	{
		ViewModel->SetTeam(GetTeam());
	}
}

void UMinimapComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto WorldLocation = GetWorldLocation();
	ViewModel->SetWorldLocation(WorldLocation);

	auto PC = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC && PC->PlayerStateReady.IsComplete())
	{
		ViewModel->SetVisible(IsVisible(PC->GetTeam()));
	}
}
