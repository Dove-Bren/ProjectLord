// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/RewardFlag.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/LordGameState.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Units/Unit.h"
#include "UI/ViewModels/RewardFlagViewModel.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"
#include "UI/ViewModels/Generic/AppealMetricsViewModel.h"

ARewardFlag::ARewardFlag()
{
	SelectionComponent = CreateDefaultSubobject<USelectionComponent>("Selection");
	SelectionComponent->SetSelectable(true);
}

void ARewardFlag::BeginPlay()
{
	Super::BeginPlay();

	ViewModel = CreateLordVM<UVMRewardFlag>(this);
	ViewModel->SetType(GetFlagType());
	ViewModel->SetTeam(GetTeam());
	ViewModel->SetReward(GetReward());
	ViewModel->SetAttachedUnit(IsValid(GetAttachedUnit()) ? GetAttachedUnit()->GetUnitVM() : nullptr);

	SelectionComponent->SetName(GetFlagName());
	SelectionComponent->SetDesc(GetFlagDescription());
	SelectionComponent->SetTeam(GetTeam());
	UVMGold* GoldVM = CreateLordVM<UVMGold>(this);
	OnRewardFlagRewardChanged.AddWeakLambda(this, [GoldVM](ARewardFlag* Flag, int NewGold) {
		GoldVM->SetGold(NewGold);
	});
	GoldVM->SetGold(GetReward());
	SelectionComponent->SetGoldVM(GoldVM);

	UVMAppealMetrics* AppealVM = CreateLordVM<UVMAppealMetrics>(this);
	OnRewardFlagInterestChanged.AddWeakLambda(this, [AppealVM](ARewardFlag* Flag, int NewCount)
	{
		AppealVM->SetInterestedCount(NewCount);
	});
	AppealVM->SetInterestedCount(GetNumInterestedUnits());
	SelectionComponent->SetAppealVM(AppealVM);
}

void ARewardFlag::EndPlay(EEndPlayReason::Type Reason)
{
	auto World = GetWorld();
	if (ensure(World))
	{
		auto State = World->GetGameState<ALordGameState>();
		auto TeamState = State->GetTeam(GetTeam());
		if (TeamState)
		{
			TeamState->RemoveFlag(this);
		}
	}

	Super::EndPlay(Reason);
}

void ARewardFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, FlagType);
	DOREPLIFETIME(ThisClass, Team);
	DOREPLIFETIME(ThisClass, Reward);
	DOREPLIFETIME(ThisClass, AttachedUnit);
	DOREPLIFETIME(ThisClass, InterestedUnits);
}

void ARewardFlag::SetFlagType(ERewardFlagType InType)
{
	if (InType != FlagType)
	{
		FlagType = InType;
		ViewModel->SetType(InType);
	}
	
}

void ARewardFlag::SetTeam(EGameTeam InTeam)
{
	if (InTeam != Team)
	{
		const auto OldValue = Team;
		Team = InTeam;
		OnRep_Team(OldValue);
	}
}

void ARewardFlag::SetReward(int InReward)
{
	if (InReward != Reward)
	{
		const auto OldValue = Reward;
		Reward = InReward;
		OnRep_Reward(OldValue);
	}
}

void ARewardFlag::SetAttachedUnit(AUnit* InUnit)
{
	if (InUnit != AttachedUnit)
	{
		if (AttachedUnit)
		{
			FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
			DetachFromActor(Rules);
			if (IsValid(AttachedUnit))
			{
				AttachedUnit->OnUnitDeath.RemoveAll(this);
			}
		}
		const auto OldValue = AttachedUnit;
		AttachedUnit = InUnit;
		if (AttachedUnit)
		{
			FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
			AttachToActor(AttachedUnit, Rules);
			SetActorRelativeLocation(FVector(0, 0, 500));
			AttachedUnit->OnUnitDeath.AddDynamic(this, &ARewardFlag::OnUnitDeath);
		}
		OnRep_AttachedUnit(OldValue);
	}
}

void ARewardFlag::AddInterestedUnit(AUnit* Unit)
{
	const auto OldValue = GetNumInterestedUnits();
	InterestedUnits.Add(Unit);
	InterestedUnitCount = InterestedUnits.Num();
	OnRep_InterestedUnits(OldValue);
}

void ARewardFlag::RemoveInterestedUnit(AUnit* Unit)
{
	if (InterestedUnits.Remove(Unit))
	{
		InterestedUnitCount = InterestedUnits.Num();
		OnRep_InterestedUnits(GetNumInterestedUnits()+1);
	}
}

void ARewardFlag::OnRep_Team(EGameTeam OldValue)
{
	ViewModel->SetTeam(GetTeam());
	SelectionComponent->SetTeam(GetTeam());
}

void ARewardFlag::OnRep_Reward(int OldValue)
{
	ViewModel->SetReward(GetReward());
	OnRewardFlagRewardChanged.Broadcast(this, GetReward());
}

void ARewardFlag::OnRep_AttachedUnit(AUnit* OldValue)
{
	ViewModel->SetAttachedUnit(IsValid(AttachedUnit) ? AttachedUnit->GetUnitVM() : nullptr);
}

void ARewardFlag::OnRep_InterestedUnits(int OldValue)
{
	ViewModel->SetInterestedCount(GetNumInterestedUnits());
	OnRewardFlagInterestChanged.Broadcast(this, GetNumInterestedUnits());
}

void ARewardFlag::OnUnitDeath(AUnit* Unit)
{
	if (Unit == AttachedUnit)
	{
		AttachedUnit->OnUnitDeath.RemoveAll(this);
		Destroy();
	}
}

FText ARewardFlag::GetFlagName_Implementation() const
{
	return FText::GetEmpty();
}

FText ARewardFlag::GetFlagDescription_Implementation() const
{
	return FText::GetEmpty();
}

