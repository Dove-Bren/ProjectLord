// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/QueuedAction.h"

#include "LordLogging.h"
#include "Gameplay/GameTeam.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/LordPlayerController.h"
#include "Gameplay/Buildings/GoodBuilding.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpell.h"
#include "Gameplay/Units/UnitTypes.h"
#include "UI/ToastNotification.h"

void UQueuedGoodAction::Init(int InGold, FGoodOffer InOffer)
{
	Icon = InOffer.Good->GetIcon();
	RefundGold = InGold;
	Good = InOffer;
}

void UQueuedSpellAction::Init(int InGold, UPlayerSpell* InSpell)
{
	Spell = InSpell;
	Icon = Spell->GetSpellIcon();
	RefundGold = InGold;
}

void UQueuedRecruitAction::Init(int InGold, UUnitType* InType)
{
	Icon = InType->UnitIcon;
	RefundGold = InGold;
	RecruitType = InType;
}

void UQueuedAction::Perform(AGoodBuilding* Building)
{
	UE_LOG(LordBuilding, Error, TEXT("Queued Action does not have a Perform action bound"));
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
	Building->AddBuildingPlayerSpell(SpellToAdd);

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
