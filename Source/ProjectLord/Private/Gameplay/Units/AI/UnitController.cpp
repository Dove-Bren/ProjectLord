// Copyright (c) Skyler Manzanares. All Rights Reserved.

#include "Gameplay/Units/AI/UnitController.h"

AUnitController::AUnitController()
{
}

void AUnitController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	auto BT = GetBehaviorTree();
	RunBehaviorTree(BT);
}

UBehaviorTree* AUnitController::GetBehaviorTree_Implementation() const
{
	checkf(false, TEXT("Controller did not implement GetBehaviorTree"));
	return nullptr;
}