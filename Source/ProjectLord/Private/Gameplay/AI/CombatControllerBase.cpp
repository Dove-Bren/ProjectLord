// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/AI/CombatControllerBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "Gameplay/Combat/CombatComponent.h"

void ACombatControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ACombatControllerBase::OnBehaviorTreeStarted()
{
	auto BB = GetBlackboardComponent();
	auto AttackTargetKey = BB->GetKeyID(TEXT("AttackTargetCombatComponent"));

	BB->RegisterObserver(AttackTargetKey, this, FOnBlackboardChangeNotification::CreateUObject(this, &ThisClass::OnBBTargetChanged));
	OnBBTargetChanged(*BB, AttackTargetKey);

	auto Combat = GetPawn()->GetComponentByClass<UCombatComponent>();
	if (ensure(Combat))
	{
		Combat->OnAttackReceived.AddDynamic(this, &ThisClass::OnPawnAttacked);
	}
}

EBlackboardNotificationResult ACombatControllerBase::OnBBTargetChanged(const UBlackboardComponent& BB, FBlackboard::FKey KeyID)
{
	auto TargetComp = BB.GetValue<UBlackboardKeyType_Object>(KeyID);
	SetTarget(Cast<UCombatComponent>(TargetComp));
	return EBlackboardNotificationResult::ContinueObserving;
}

void ACombatControllerBase::NotifyPawnDied()
{

}

void ACombatControllerBase::OverrideTarget(UCombatComponent* InTarget)
{
	auto BB = GetBlackboardComponent();
	FName AttackTargetKey = TEXT("AttackTargetCombatComponent");

	BB->SetValueAsObject(AttackTargetKey, InTarget);
}

void ACombatControllerBase::OnPawnAttacked(AActor* AttackingActor, UCombatComponent* AttackingCombatComponent)
{
	auto BB = GetBlackboardComponent();
	BB->SetValueAsObject(TEXT("RecentRevengeCombatComponent"), AttackingCombatComponent);
}