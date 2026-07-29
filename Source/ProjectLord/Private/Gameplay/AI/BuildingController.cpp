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
}

UBehaviorTree* ABuildingController::GetBehaviorTree_Implementation() const
{
	checkf(false, TEXT("Controller did not implement GetBehaviorTree"));
	return nullptr;
}

UCombatComponent* ABuildingController::GetTargetComponent_Implementation() const
{
	const auto BB = GetBlackboardComponent();
	auto TargetComp = BB->GetValueAsObject(TEXT("AttackTargetCombatComponent"));
	return Cast<UCombatComponent>(TargetComp);
}