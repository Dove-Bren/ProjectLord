// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/QueuedAction.h"

#include "LordLogging.h"
#include "Gameplay/Buildings/GoodBuilding.h"
#include "Gameplay/Units/UnitTypes.h"

void UQueuedGoodAction::Init(int InGold, FGoodOffer InOffer)
{
	Icon = InOffer.Good->GetIcon();
	RefundGold = InGold;
	Good = InOffer;
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
	Building->AddGoodOffer(GetGood());
}

void UQueuedRecruitAction::Perform(AGoodBuilding* Building)
{
	Building->RecruitNewUnit(GetRecruitType());
}
