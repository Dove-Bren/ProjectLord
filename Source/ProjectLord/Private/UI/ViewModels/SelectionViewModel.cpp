// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/SelectionViewModel.h"

#include "Gameplay/SelectionAction.h"
#include "Gameplay/SelectionComponent.h"

void UVMSelection::Reset(bool bTriggerUpdate)
{
	SetPresent(false);

	SetSelectionName({});
	SetTeam(EGameTeam::Neutral);
	SetIcon(nullptr);

    GoldVM = nullptr;
    ActionVM = nullptr;
    CombatDataVM = nullptr;
    LevelVM = nullptr;
    ProgressQueueVM = nullptr;
    SlotsVM = nullptr;
    ActionTreeVM = nullptr;
	CategoryVM = nullptr;

    if (bTriggerUpdate)
    {
        TriggerSelectionChange();
    }
}

void UVMSelection::SetFromSelection(const USelectionComponent* Selection, FSelectionActionContext Context, bool bIncludeActions, bool bTriggerUpdate)
{
	Reset(!Selection);
	if (Selection)
	{
		SetIcon(Selection->GetIcon());
		SetSelectionName(Selection->GetName());
		SetSelectionDescription(Selection->GetDescription());
		SetCustomName(Selection->GetCustomName());
		SetTeam(Selection->GetTeam());

		ActionVM = Selection->GetActionVM();
		CombatDataVM = Selection->GetCombatDataVM();
		GoldVM = Selection->GetGoldVM();
		LevelVM = Selection->GetLevelVM();
		ProgressQueueVM = Selection->GetQueueVM();
		SlotsVM = Selection->GetSlotsVM();
		ActionTreeVM = Selection->GetActionTreeVM();
		CategoryVM = Selection->GetCategoryVM();
		//TargetVM = Selection->GetTargetVM();

		SetPresent(true);

		if (bTriggerUpdate)
		{
			TriggerSelectionChange();
		}
	}
}

void UVMSelection::SetFromStaticElement(FStaticSelection StaticElement, bool bTriggerUpdate)
{
	Reset(false);
	SetSelectionName(StaticElement.Name);
	SetSelectionDescription(StaticElement.Description);

	SetPresent(true);

	if (bTriggerUpdate)
	{
		TriggerSelectionChange();
	}
}
