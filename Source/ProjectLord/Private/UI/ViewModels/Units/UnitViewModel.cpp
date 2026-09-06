// Copyright (c) Skyler Manzanares. All Rights Reserved.

#include "UI/ViewModels/Units/UnitViewModel.h"

#include "Kismet/GameplayStatics.h"

#include "AbilitySystemComponent.h"
#include "Gameplay/LordPlayerController.h"
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

void UVMUnit::SelectUnit()
{
    if (IsValid(Model))
    {
        auto PC = UGameplayStatics::GetPlayerController(Model, 0);
        if (auto LordPC = Cast<ALordPlayerController>(PC))
        {
            LordPC->SetSelection(Model->GetComponentByClass<USelectionComponent>());
        }
    }
}
