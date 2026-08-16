// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/LordPlayerStateViewModel.h"

#include "Gameplay/LordPlayerController.h"
#include "Gameplay/LordPlayerState.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Buildings/Building.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"

UVMLordPlayerState::UVMLordPlayerState()
{
	GoldVM = CreateDefaultSubobject<UVMGold>(TEXT("Gold SubVM"));
}

void UVMLordPlayerState::Setup(ALordPlayerState* State)
{
	ParentState = State;

	SetTeam(State->GetPlayerTeam());

	State->OnPlayerGoldChanged.AddWeakLambda(this, [this](int NewGold)
		{
			GoldVM->SetGold(NewGold);
		});
	GoldVM->SetGold(State->GetPlayerGold());
}

void UVMLordPlayerState::SelectCastle()
{
	if (ensure(ParentState.IsValid()))
	{
		auto Controller = GetWorld()->GetFirstPlayerController<ALordPlayerController>();
		auto Castle = ParentState->GetPlayerCastle();
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
