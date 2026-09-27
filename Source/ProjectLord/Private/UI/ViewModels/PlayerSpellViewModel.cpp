// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/PlayerSpellViewModel.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/LordPlayerController.h"
#include "Gameplay/SelectionComponent.h"

void UVMPlayerSpell::StartCasting(const UObject* WorldContextObject)
{
	if (auto Spell = SpellToCast.Pin())
	{
		auto PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
		if (auto LordPC = Cast<ALordPlayerController>(PC))
		{
			LordPC->CastSpell(Spell.Get());
		}
	}
}
