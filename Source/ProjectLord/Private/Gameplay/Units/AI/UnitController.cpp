// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/AI/UnitController.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Gameplay/Combat/CombatComponent.h"

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

UCombatComponent* AUnitController::GetTargetComponent_Implementation() const
{
	const auto BB = GetBlackboardComponent();
	auto TargetComp = BB->GetValueAsObject(TEXT("AttackTargetCombatComponent"));
	return Cast<UCombatComponent>(TargetComp);
}