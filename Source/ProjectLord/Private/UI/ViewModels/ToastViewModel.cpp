// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/ToastViewModel.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/LordPlayerController.h"
#include "Gameplay/SelectionComponent.h"

void UVMToast::SelectSource()
{
	if (IsValid(SelectSourceComponent))
	{
		auto PC = UGameplayStatics::GetPlayerController(SelectSourceComponent, 0);
		if (auto LordPC = Cast<ALordPlayerController>(PC))
		{
			LordPC->SetSelection(SelectSourceComponent);
			LordPC->SetFocusedActor(SelectSourceComponent->GetOwner());
		}
	}
}
