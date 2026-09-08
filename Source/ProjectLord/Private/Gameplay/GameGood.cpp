// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/GameGood.h"

#include "UI/ViewModels/GameGoodViewModel.h"

UVMGameGood* UGameGood::GetOrCreateViewModel()
{
	if (!ViewModel)
	{
		ViewModel = CreateLordVM<UVMGameGood>(this);
		ViewModel->SetName(GetName());
		ViewModel->SetDescription(GetDescription());
		ViewModel->SetIcon(GetIcon());
	}
	return ViewModel;
}
