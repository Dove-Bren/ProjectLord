// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/RewardFlag.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/LordGameState.h"
#include "Gameplay/Units/Unit.h"
#include "UI/ViewModels/RewardFlagViewModel.h"

ARewardFlag::ARewardFlag()
{

}

void ARewardFlag::BeginPlay()
{
	Super::BeginPlay();

	ViewModel = CreateLordVM<UVMRewardFlag>(this);
	ViewModel->SetType(GetFlagType());
	ViewModel->SetTeam(GetTeam());
	ViewModel->SetReward(GetReward());
	ViewModel->SetAttachedUnit(IsValid(GetAttachedUnit()) ? GetAttachedUnit()->GetUnitVM() : nullptr);
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
		Team = InTeam;
		ViewModel->SetTeam(InTeam);
	}
}

void ARewardFlag::SetReward(int InReward)
{
	if (InReward != Reward)
	{
		Reward = InReward;
		ViewModel->SetReward(InReward);
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
		AttachedUnit = InUnit;
		ViewModel->SetAttachedUnit(IsValid(InUnit) ? InUnit->GetUnitVM() : nullptr);
		if (AttachedUnit)
		{
			FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
			AttachToActor(AttachedUnit, Rules);
			SetActorRelativeLocation(FVector(0, 0, 500));
			AttachedUnit->OnUnitDeath.AddDynamic(this, &ARewardFlag::OnUnitDeath);
		}
	}
}

void ARewardFlag::OnUnitDeath(AUnit* Unit)
{
	if (Unit == AttachedUnit)
	{
		AttachedUnit->OnUnitDeath.RemoveAll(this);
		Destroy();
	}
}

