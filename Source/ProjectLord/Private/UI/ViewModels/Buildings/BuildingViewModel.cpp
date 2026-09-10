// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Buildings/BuildingViewModel.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/LordPlayerController.h"
#include "Gameplay/Buildings/Building.h"

void UVMBuilding::FocusBuilding()
{
    if (auto Pinned = Model.Pin())
    {
        auto PC = UGameplayStatics::GetPlayerController(Pinned.Get(), 0);
        if (auto LordPC = Cast<ALordPlayerController>(PC))
        {
            LordPC->SetFocusedActor(Pinned.Get());
        }
    }
}
