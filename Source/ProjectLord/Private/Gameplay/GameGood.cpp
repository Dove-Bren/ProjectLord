// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/GameGood.h"

#include "UI/ViewModels/GameGoodViewModel.h"

void UGameGood::PostInitProperties()
{
	Super::PostInitProperties();

	ViewModel = CreateLordVM<UVMGameGood>(this);
	ViewModel->SetName(GetName());
	ViewModel->SetDescription(GetDescription());
	ViewModel->SetIcon(GetIcon());
}
