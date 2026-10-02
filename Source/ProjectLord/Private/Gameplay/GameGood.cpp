// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/GameGood.h"

#include "Gameplay/Units/HeroEquipment.h"
#include "UI/ViewModels/GameGoodViewModel.h"
#include "UI/ViewModels/Generic/StatModifiersViewModel.h"

UVMGameGood* UGameGood::GetOrCreateViewModel()
{
	if (!ViewModel)
	{
		ViewModel = CreateLordVM<UVMGameGood>(this);
		ViewModel->SetName(GetName());
		ViewModel->SetDescription(GetDescription());
		ViewModel->SetIcon(GetIcon());

		if (GetAbility())
		{
			ViewModel->SetModifiers(UVMStatModifiers::Make(this, GetAbility().GetDefaultObject()->GetAbilityModifierPreview()));
		}
		else if (GetItemDef())
		{
			ViewModel->SetModifiers(UVMStatModifiers::Make(this, GetItemDef()->GetPreviewModifiers()));
		}
	}
	return ViewModel;
}
