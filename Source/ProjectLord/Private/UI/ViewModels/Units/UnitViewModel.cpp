// Copyright (c) Skyler Manzanares. All Rights Reserved.

#include "UI/ViewModels/Units/UnitViewModel.h"

#include "AbilitySystemComponent.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Units/Unit.h"


UVMUnit* UVMUnit::CreateForUnit(AUnit* Unit)
{
    UVMUnit* UnitVM = CreateLordVM<UVMUnit>(Unit);
    if (IsValid(UnitVM))
    {
        UnitVM->Model = Unit;
    }

    return UnitVM;
}
