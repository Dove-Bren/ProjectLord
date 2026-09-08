// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Units/HeroViewModel.h"

#include "Gameplay/Units/HeroBase.h"

/*static*/ UVMHero* UVMHero::Make(AHeroBase* Hero)
{
    UVMHero* UnitVM = CreateLordVM<UVMHero>(Hero);
    if (IsValid(UnitVM))
    {
        UnitVM->Model = Hero;
    }

    return UnitVM;
}
