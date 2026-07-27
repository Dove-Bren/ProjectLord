// Copyright (c) Skyler Manzanares. All Rights Reserved.

#include "UI/ViewModels/Units/UnitViewModel.h"

#include "AbilitySystemComponent.h"
#include "Gameplay/Combat/Ability/LordUnitAttributeSet.h"
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

void UVMUnit::InitializeAttributeListeners(UAbilitySystemComponent* Component, ULordUnitAttributeSet* Attributes)
{
    // TODO: Simplify boilerplate
    bool bFound;

    Component->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute())
        .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
            {
                SetHealth(ChangeData.NewValue);
            });
    SetHealth(Component->GetGameplayAttributeValue(Attributes->GetHealthAttribute(), bFound));

    Component->GetGameplayAttributeValueChangeDelegate(Attributes->GetMaxHealthAttribute())
        .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
            {
                SetMaxHealth(ChangeData.NewValue);
            });
    SetMaxHealth(Component->GetGameplayAttributeValue(Attributes->GetMaxHealthAttribute(), bFound));
}
