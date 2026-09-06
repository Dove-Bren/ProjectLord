// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/RewardFlag.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/MinimapComponent.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/HeroBase.h"
#include "Gameplay/Units/Unit.h"
#include "UI/ViewModels/RewardFlagViewModel.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"
#include "UI/ViewModels/Generic/AppealMetricsViewModel.h"
#include "UI/ViewModels/Generic/CombatDataViewModel.h"

ARewardFlag::ARewardFlag()
{
	SelectionComponent = CreateDefaultSubobject<USelectionComponent>(TEXT("Selection"));
	SelectionComponent->SetSelectable(true);

	MinimapComponent = CreateDefaultSubobject<UMinimapComponent>(TEXT("Minimap"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ARewardFlag::BeginPlay()
{
	Super::BeginPlay();

	ViewModel = CreateLordVM<UVMRewardFlag>(this);
	ViewModel->SetType(GetFlagType());
	ViewModel->SetTeam(GetTeam());
	ViewModel->SetReward(GetReward());
	ViewModel->SetAttachedCombatComponent(IsValid(GetAttachedComponent()) ? UVMCombatData::Make(this, GetAttachedComponent()) : nullptr);

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

	MinimapComponent->SetTeam(GetTeam());
}

void ARewardFlag::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch (GetFlagType())
	{
	case ERewardFlagType::Explore:
		ExploreTick(DeltaSeconds);
		break;
	case ERewardFlagType::Attack:
		AttackTick(DeltaSeconds);
		break;
	case ERewardFlagType::Defend:
		DefendTick(DeltaSeconds);
		break;
	case ERewardFlagType::Fear:
		FearTick(DeltaSeconds);
		break;
	}
}

void ARewardFlag::ExploreTick(float DeltaSeconds)
{
	// Explore flags listen for a unit to be close
	bool bComplete = false;
	for (auto Unit : InterestedUnits)
	{
		if (FVector::DistSquaredXY(GetActorLocation(), Unit->GetActorLocation()) <= 500 * 500)
		{
			bComplete = true;
			break;
		}
	}

	if (bComplete)
	{
		PayNearbyInterestedHeroes(GetReward(), 1000); // Larger range for pay to split on near-ties
		SetReward(0);
		Destroy();
	}
}

void ARewardFlag::DefendTick(float DeltaSeconds)
{
	if (GetNumInterestedUnits() <= 0)
	{
		// Nobody defending, so reset timer
		LastPayTime = 0;
	}
	else
	{
		const auto Now = GetGameTimeSinceCreation();
		if (LastPayTime == 0)
		{
			LastPayTime = Now;
		}
		else if (Now - LastPayTime > 15)
		{
			LastPayTime = Now;
			PayNearbyInterestedHeroes(100, 2000);

			const int NewReward = GetReward() - 100;
			SetReward(NewReward);
			if (NewReward <= 0)
			{
				Destroy();
			}
		}
	}
}

void ARewardFlag::FearTick(float DeltaSeconds)
{
	// TODO
}

void ARewardFlag::AttackTick(float DeltaSeconds)
{
	// Nothing to do
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

	auto UnitsCopy = GetInterestedUnits();
	for (auto Unit : UnitsCopy)
	{
		if (auto Hero = Cast<AHeroBase>(Unit))
		{
			Hero->ClearFlagTarget();
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
	DOREPLIFETIME(ThisClass, AttachedComponent);
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

void ARewardFlag::SetAttachedComponent(UCombatComponent* InComponent)
{
	if (InComponent != AttachedComponent)
	{
		if (AttachedComponent)
		{
			FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
			DetachFromActor(Rules);
			if (IsValid(AttachedComponent))
			{
				AttachedComponent->OnDeath.RemoveAll(this);
			}
		}
		const auto OldValue = AttachedComponent;
		AttachedComponent = InComponent;
		if (AttachedComponent)
		{
			FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
			AttachToActor(AttachedComponent->GetOwner(), Rules);
			SetActorRelativeLocation(FVector(0, 0, 500));
			AttachedComponent->OnDeath.AddDynamic(this, &ARewardFlag::OnUnitDeath);
		}
		OnRep_AttachedComponent(OldValue);
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

FVector ARewardFlag::GetGroundLocation() const
{
	if (IsValid(AttachedComponent))
	{
		return AttachedComponent->GetOwner()->GetActorLocation();
	}

	FHitResult Result;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(Result, GetActorLocation(), GetActorLocation() - FVector(0, 0, 10000), ECC_Camera, CollisionParams))
	{
		return Result.Location;
	}

	return GetActorLocation() - FVector(0, 0, 500);
}

bool ARewardFlag::PayHeroes(int Amount, const TArray<AHeroBase*>& Heroes)
{
	if (Heroes.IsEmpty())
	{
		return false;
	}

	int GoldEach = FMath::Max(1, FMath::CeilToInt((float)Amount / (float)Heroes.Num()));
	for (auto Hero : Heroes)
	{
		Hero->AwardGold(GoldEach);
	}
	return true;
}

bool ARewardFlag::PayNearbyInterestedHeroes(int Amount, float Range)
{
	TArray<AHeroBase*> Heroes;
	const double RangeSqr = Range * Range;
	const FVector Root = GetActorLocation();
	for (auto Unit : InterestedUnits)
	{
		if (auto Hero = Cast<AHeroBase>(Unit))
		{
			if (FVector::DistSquaredXY(Hero->GetActorLocation(), Root) <= RangeSqr)
			{
				Heroes.Add(Hero);
			}
		}
	}
	return PayHeroes(Amount, Heroes);
}

void ARewardFlag::OnRep_Team(EGameTeam OldValue)
{
	ViewModel->SetTeam(GetTeam());
	SelectionComponent->SetTeam(GetTeam());
	MinimapComponent->SetTeam(GetTeam());
}

void ARewardFlag::OnRep_Reward(int OldValue)
{
	ViewModel->SetReward(GetReward());
	OnRewardFlagRewardChanged.Broadcast(this, GetReward());
}

void ARewardFlag::OnRep_AttachedComponent(UCombatComponent* OldValue)
{
	ViewModel->SetAttachedCombatComponent(IsValid(GetAttachedComponent()) ? UVMCombatData::Make(this, GetAttachedComponent()) : nullptr);
}

void ARewardFlag::OnRep_InterestedUnits(int OldValue)
{
	ViewModel->SetInterestedCount(GetNumInterestedUnits());
	OnRewardFlagInterestChanged.Broadcast(this, GetNumInterestedUnits());
}

void ARewardFlag::OnUnitDeath()
{
	if (AttachedComponent)
	{
		AttachedComponent->OnDeath.RemoveAll(this);

		if (ERewardFlagType::Attack == GetFlagType())
		{
			PayNearbyInterestedHeroes(GetReward(), 2000);
		}

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

