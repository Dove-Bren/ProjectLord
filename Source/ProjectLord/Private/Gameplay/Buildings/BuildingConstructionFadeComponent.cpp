// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/BuildingConstructionFadeComponent.h"

#include "Gameplay/Buildings/Building.h"

UBuildingConstructionFadeComponent::UBuildingConstructionFadeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bAutoActivate = true;
}

void UBuildingConstructionFadeComponent::BeginPlay()
{
	Super::BeginPlay();
	Enable();
}

void UBuildingConstructionFadeComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bDidWork = false;
	if (auto Building = GetBuilding())
	{
		if (Building->GetBuildingLevel() < Building->GetBuildingAvailableLevel())
		{
			bDidWork = true;
			float Progress = (float)Building->GetBuildingHealth() / (float)Building->GetBuildingMaxHealth();
			UpdateMesh(Progress);
		}
		else
		{
			UpdateMesh(1);
			// don't set bDidWork, so we stop ticking
		}
	}

	if (!bDidWork && --IdleTickCount <= 0)
	{
		SetComponentTickEnabled(false);
	}
}

void UBuildingConstructionFadeComponent::Enable()
{
	IdleTickCount = 10;
	SetComponentTickEnabled(true);
}

void UBuildingConstructionFadeComponent::UpdateMesh(float ConstructionProgress)
{
	static constexpr float Values[] = {.4f, .6f, .8f, 1.0f};
	// find what value to use
	float Value = Values[0];
	for (float V : Values) {
		if (V > ConstructionProgress) break;
		Value = V;
	}

	SetupMaterials();
	for (auto Material : BuildingFadeMaterials)
	{
		Material->SetScalarParameterValue(TEXT("ConstructionProgress"), Value);
	}
}

ABuilding* UBuildingConstructionFadeComponent::GetBuilding() const
{
	return Cast<ABuilding>(GetOwner());
}

void UBuildingConstructionFadeComponent::SetupMaterials()
{
	ABuilding* Building = GetBuilding();
	if (BuildingFadeMaterials.IsEmpty() && Building)
	{
		auto Mesh = Building->GetBuildingMeshComponent();
		for (int i = 0; i < Mesh->GetNumMaterials(); i++)
		{
			BuildingFadeMaterials.Add(Mesh->CreateAndSetMaterialInstanceDynamic(i));
		}
	}
}