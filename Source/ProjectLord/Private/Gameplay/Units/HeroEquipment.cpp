// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroEquipment.h"

#include "Net/UnrealNetwork.h"

#include "UI/ViewModels/ItemStackViewModel.h"

void UHeroItemStack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemDef);
	DOREPLIFETIME(ThisClass, Count);
}

void UHeroItemStack::Init(const UHeroItemDef* InItemDef, int InCount)
{
	check(IsValid(InItemDef));
	ItemDef = InItemDef;
	OnRep_Def();

	SetCount(InCount);
}

UVMItemStack* UHeroItemStack::MakeViewModel()
{
	if (IsValid(ViewModel))
	{
		return ViewModel;
	}

	ViewModel = CreateLordVM<UVMItemStack>(this);

	if (ItemDef)
	{
		OnRep_Def();
	}
	OnRep_Count();

	return ViewModel;
}

int UHeroItemStack::SetCount(int NewCount)
{
	Count = FMath::Max(0, NewCount);
	OnRep_Count();
	return GetCount();
}

void UHeroItemStack::OnRep_Def()
{
	if (ViewModel)
	{
		ViewModel->SetItemName(ItemDef->GetItemName());
		ViewModel->SetDescription(ItemDef->GetItemDescription());
		ViewModel->SetIcon(ItemDef->GetItemIcon());
		ViewModel->SetStacks(ItemDef->GetCanStack());
		ViewModel->SetPreviewModifiers(UVMStatModifiers::Make(this, ItemDef->GetPreviewModifiers()));
	}
}

void UHeroItemStack::OnRep_Count()
{
	if (ViewModel)
	{
		ViewModel->SetCount(Count);
	}
}