// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/QueuedAction.h"

#include "Net/UnrealNetwork.h"

#include "LordLogging.h"
#include "Gameplay/GameTeam.h"
#include "Gameplay/GameTeamState.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Buildings/GoodBuilding.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpell.h"
#include "Gameplay/Units/UnitTypes.h"
#include "UI/ToastNotification.h"

FQueuedAction UQueuedGoodAction::Make(UObject* Outer, int InGold, FGoodOffer InOffer)
{
	FQueuedAction Action;
	Action.Icon = InOffer.Good->GetIcon();
	Action.RefundGold = InGold;

	auto Execution = NewObject<UQueuedGoodAction>(Outer);
	Execution->Good = InOffer;
	Action.Execution = Execution;

	return Action;
}

FQueuedAction UQueuedSpellAction::Make(UObject* Outer, int InGold, UPlayerSpell* InSpell)
{
	FQueuedAction Action;
	Action.Icon = InSpell->GetSpellIcon();
	Action.RefundGold = InGold;

	auto Execution = NewObject<UQueuedSpellAction>(Outer);
	Execution->Spell = InSpell;
	Action.Execution = Execution;

	return Action;
}

FQueuedAction UQueuedRecruitAction::Make(UObject* Outer, int InGold, UUnitType* InType)
{
	FQueuedAction Action;
	Action.Icon = InType->UnitIcon;
	Action.RefundGold = InGold;

	auto Execution = NewObject<UQueuedRecruitAction>(Outer);
	Execution->RecruitType = InType;
	Action.Execution = Execution;

	return Action;
}

void UQueuedGoodAction::Perform(AGoodBuilding* Building)
{
	auto GoodOffer = GetGood();
	Building->AddGoodOffer(GoodOffer);

	// Toast!
	if (auto State = GetWorld()->GetGameState<ALordGameState>())
	{
		if (AGameTeamState* TeamState = State->GetTeam(Building->GetTeam()))
		{
			if (auto Controller = TeamState->GetPrimaryPlayerController())
			{

				Controller->AddToastNotification(FToastNotification(EToastNotificationType::ResearchComplete, GoodOffer.Good->GetIcon(), GoodOffer.Good->GetName(), Building->GetComponentByClass<USelectionComponent>()));
			}
		}
	}
}

void UQueuedSpellAction::Perform(AGoodBuilding* Building)
{
	auto SpellToAdd = GetSpell();
	Building->Server_AddBuildingPlayerSpell(SpellToAdd);

	// Toast!
	if (auto State = GetWorld()->GetGameState<ALordGameState>())
	{
		if (AGameTeamState* TeamState = State->GetTeam(Building->GetTeam()))
		{
			if (auto Controller = TeamState->GetPrimaryPlayerController())
			{
				Controller->AddToastNotification(FToastNotification(EToastNotificationType::ResearchComplete, SpellToAdd->GetSpellIcon(), SpellToAdd->GetSpellName(), Building->GetComponentByClass<USelectionComponent>()));
			}
		}
	}
}

void UQueuedRecruitAction::Perform(AGoodBuilding* Building)
{
	Building->RecruitNewUnit(GetRecruitType());
}

void UQueuedGoodAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Good);
}

void UQueuedSpellAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Spell);
}

void UQueuedRecruitAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RecruitType);
}
