// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/GameTeamStateViewModel.h"

#include "Gameplay/LordPlayerController.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Buildings/Building.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"

UVMGameTeamState::UVMGameTeamState()
{
	GoldVM = CreateDefaultSubobject<UVMGold>(TEXT("Gold SubVM"));
}

void UVMGameTeamState::Setup(AGameTeamState* State)
{
	ParentState = State;

	SetTeam(State->GetTeam());

	State->OnTeamGoldChanged.AddWeakLambda(this, [this](int NewGold)
		{
			GoldVM->SetGold(NewGold);
		});
	GoldVM->SetGold(State->GetGold());
}

void UVMGameTeamState::SelectCastle()
{
	if (ensure(ParentState.IsValid()))
	{
		auto Controller = GetWorld()->GetFirstPlayerController<ALordPlayerController>();
		auto Castle = ParentState->GetCastle();
		if (ensure(Controller && Castle))
		{
			auto Selection = Castle->GetComponentByClass<USelectionComponent>();
			if (Selection)
			{
				Controller->SetSelection(Selection);
			}
		}
	}
}
