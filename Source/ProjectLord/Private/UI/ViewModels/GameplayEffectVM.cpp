// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/GameplayEffectVM.h"

#include "Gameplay/Combat/GameplayEffect/VisibleGameplayEffect.h"

UVMGameplayEffect::UVMGameplayEffect()
{
}

void UVMGameplayEffect::Setup(const UVisibleGameplayEffect* Effect)
{
	SetName(Effect->GetName());
	SetDescription(Effect->GetDescription());
	SetIcon(Effect->GetIcon());
}
