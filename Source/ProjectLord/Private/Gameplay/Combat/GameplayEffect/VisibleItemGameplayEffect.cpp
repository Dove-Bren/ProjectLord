// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/GameplayEffect/VisibleItemGameplayEffect.h"

#include "Gameplay/Units/HeroEquipment.h"

void UVisibleItemGameplayEffect::PostInitProperties()
{
	Super::PostInitProperties();

	if (IsValid(ItemDef))
	{
		EffectName = ItemDef->GetItemName();
		EffectDescription = ItemDef->GetItemDescription();
		Icon = ItemDef->GetItemIcon();
	}
}
