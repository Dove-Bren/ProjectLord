// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroEquipment.h"

#include "UI/ViewModels/ItemStackViewModel.h"

void UHeroItemStack::Init(const UHeroItemDef* InItemDef, int InCount)
{
	ViewModel = CreateLordVM<UVMItemStack>(this);

	check(IsValid(InItemDef));
	ItemDef = InItemDef;
	ViewModel->SetItemName(ItemDef->GetItemName());
	ViewModel->SetDescription(ItemDef->GetItemDescription());
	ViewModel->SetIcon(ItemDef->GetItemIcon());
	ViewModel->SetStacks(ItemDef->GetCanStack());
	ViewModel->SetPreviewModifiers(UVMStatModifiers::Make(this, ItemDef->GetPreviewModifiers()));

	SetCount(InCount);
}

int UHeroItemStack::SetCount(int NewCount)
{
	Count = FMath::Max(0, NewCount);
	ViewModel->SetCount(Count);
	return GetCount();
}
