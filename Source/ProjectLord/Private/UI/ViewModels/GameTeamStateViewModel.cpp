// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/GameTeamStateViewModel.h"

#include "Gameplay/LordPlayerController.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Units/RewardFlag.h"
#include "Gameplay/Units/Unit.h"
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

	auto UpdateUnits = [this]()
		{
			TArray<UVMUnit*> VMs;
			if (auto TeamState = ParentState.Pin())
			{
				for (auto Unit : TeamState->GetUnits())
				{
					VMs.Add(Unit->GetUnitVM());
				}
			}
			UpdateTeamUnits(MoveTemp(VMs));
		};
	State->OnTeamUnitsChanged.AddWeakLambda(this, UpdateUnits);
	UpdateUnits();

	auto UpdateFlags = [this]()
		{
			TArray<UVMRewardFlag*> VMs;
			if (auto TeamState = ParentState.Pin())
			{
				for (auto Flag : TeamState->GetFlags())
				{
					VMs.Add(Flag->GetViewModel());
				}
			}
			UpdateTeamFlags(MoveTemp(VMs));
		};
	State->OnTeamFlagsChanged.AddWeakLambda(this, UpdateFlags);
	UpdateFlags();
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
