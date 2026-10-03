// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/PlayerSpellViewModel.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Interaction/SelectionComponent.h"

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
