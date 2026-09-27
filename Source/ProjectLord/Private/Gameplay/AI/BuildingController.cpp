// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/AI/BuildingController.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Gameplay/Combat/CombatComponent.h"

ABuildingController::ABuildingController()
{
}

void ABuildingController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	auto BT = GetBehaviorTree();
	RunBehaviorTree(BT);
	OnBehaviorTreeStarted();
}

UBehaviorTree* ABuildingController::GetBehaviorTree_Implementation() const
{
	checkf(false, TEXT("Controller did not implement GetBehaviorTree"));
	return nullptr;
}