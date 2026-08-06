// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/SelectionViewModel.h"

#include "Gameplay/SelectionComponent.h"
#include "Gameplay/SelectionAction.h"
#include "UI/ViewModels/SelectionActionViewModel.h"

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
    Actions.Empty();

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
		//TargetVM = Selection->GetTargetVM();

		if (bIncludeActions)
		{
			for (auto Action : Selection->GetAvailableActions())
			{
				UVMSelectionAction* AvailableActionVM;
				if (!IsValid(Action) || Action->IsHidden(Context))
				{
					AvailableActionVM = nullptr;
				}
				else
				{
					AvailableActionVM = UVMSelectionAction::Make(this, Action);
				}
				Actions.Add(AvailableActionVM);
			}
		}

		SetPresent(true);

		if (bTriggerUpdate)
		{
			TriggerSelectionChange();
		}
	}
}
