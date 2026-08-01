// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/QueuedAction.h"

#include "LordLogging.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Units/UnitTypes.h"

void UQueuedGoodAction::Init(int InGold, FGoodOffer InOffer)
{
	Icon = InOffer.Good->GetIcon();
	RefundGold = InGold;
	Good = InOffer;
}

void UQueuedRecruitAction::Init(int InGold, UCreatureType* InType)
{
	Icon = InType->CreatureIcon;
	RefundGold = InGold;
	RecruitType = InType;
}

void UQueuedAction::Perform(ABuilding* Building)
{
	UE_LOG(LordBuilding, Error, TEXT("Queued Action does not have a Perform action bound"));
}

void UQueuedGoodAction::Perform(ABuilding* Building)
{
	Building->AddGoodOffer(GetGood());
}

void UQueuedRecruitAction::Perform(ABuilding* Building)
{
	Building->RecruitNewUnit(GetRecruitType());
}
