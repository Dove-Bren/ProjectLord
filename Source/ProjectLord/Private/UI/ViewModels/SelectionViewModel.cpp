// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/SelectionViewModel.h"

void UVMSelection::Reset(bool bTriggerUpdate)
{
	SetSelectionName({});
	SetTeam(EGameTeam::Neutral);
	SetIcon(nullptr);

    GoldVM = nullptr;
    ActionVM = nullptr;
    CombatDataVM = nullptr;
    LevelVM = nullptr;
    ProgressQueueVM = nullptr;

    if (bTriggerUpdate)
    {
        TriggerSelectionChange();
    }
}
