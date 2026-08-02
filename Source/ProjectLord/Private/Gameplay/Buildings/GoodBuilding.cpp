// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/GoodBuilding.h"

#include "Gameplay/GameGood.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Buildings/BuildingActionQueue.h"
#include "Gameplay/Buildings/QueuedAction.h"
#include "UI/ViewModels/Generic/ProgressQueueViewModel.h"

AGoodBuilding::AGoodBuilding()
{

    QueueComponent = CreateDefaultSubobject<UBuildingActionQueueComponent>(TEXT("Queue"));
}

void AGoodBuilding::BeginPlay()
{
    Super::BeginPlay();

    QueueComponent->OnActionReady.AddDynamic(this, &AGoodBuilding::OnQueueActionReady);
}

bool AGoodBuilding::HasGood(UGameGood* GoodType) const
{
    for (const auto& Good : Goods)
    {
        if (Good.Good == GoodType)
        {
            return true;
        }
    }

    return false;
}

void AGoodBuilding::AddGoodOffer(FGoodOffer InOffer)
{
    if (!HasGood(InOffer.Good))
    {
        Goods.Add(InOffer);
    }
}

void AGoodBuilding::SetupBaseGoods()
{
    for (const auto& Good : DefaultGoods)
    {
        Goods.Add(Good);
    }
}

void AGoodBuilding::SetupSelectionData(USelectionComponent* InSelectionComponent)
{
    Super::SetupSelectionData(InSelectionComponent);

    InSelectionComponent->SetQueueVM(QueueComponent->GetViewModel());
}

void AGoodBuilding::OnQueueActionReady(UQueuedAction* Action)
{
    Action->Perform(this);
}

int AGoodBuilding::GetResidentsInQueue(const UUnitType* Type) const
{
    int Count = 0;
    for (const auto Action : QueueComponent->GetQueue())
    {
        if (auto RecruitAction = Cast<UQueuedRecruitAction>(Action))
        {
            if (RecruitAction->GetRecruitType() == Type)
            {
                Count++;
            }
        }
    }
    return Count;
}