// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/AI/UnitController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "Gameplay/Combat/CombatComponent.h"

AUnitController::AUnitController()
{
}

void AUnitController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	auto BT = GetBehaviorTree();
	RunBehaviorTree(BT);

	auto BB = GetBlackboardComponent();
	auto AttackTargetKey = BB->GetKeyID(TEXT("AttackTargetCombatComponent"));

	BB->RegisterObserver(AttackTargetKey, this, FOnBlackboardChangeNotification::CreateUObject(this, &AUnitController::OnBBTargetChanged));
	OnBBTargetChanged(*BB, AttackTargetKey);

	auto Combat = InPawn->GetComponentByClass<UCombatComponent>();
	if (ensure(Combat))
	{
		Combat->OnAttackReceived.AddDynamic(this, &AUnitController::OnUnitAttacked);
	}
}

UBehaviorTree* AUnitController::GetBehaviorTree_Implementation() const
{
	checkf(false, TEXT("Controller did not implement GetBehaviorTree"));
	return nullptr;
}

EBlackboardNotificationResult AUnitController::OnBBTargetChanged(const UBlackboardComponent& BB, FBlackboard::FKey KeyID)
{
	auto TargetComp = BB.GetValue<UBlackboardKeyType_Object>(KeyID);
	SetTarget(Cast<UCombatComponent>(TargetComp));
	return EBlackboardNotificationResult::ContinueObserving;
}

void AUnitController::NotifyUnitDied()
{
	
}

void AUnitController::OnUnitAttacked(AActor* AttackingActor, UCombatComponent* AttackingCombatComponent)
{
	auto BB = GetBlackboardComponent();
	BB->SetValueAsObject(TEXT("RecentRevengeCombatComponent"), AttackingCombatComponent);
}