// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Generic/SummarySlotsViewModel.h"

#include "Gameplay/Units/HeroEquipment.h"
#include "Gameplay/Units/UnitTypes.h"
#include "UI/WidgetBlueprintClassRegistry.h"

UVMSummarySlots::UVMSummarySlots()
{

}

void UVMSummarySlots::Init()
{
	Slots.Add(UVMSummarySlot::MakeEmpty(this));
	Slots.Add(UVMSummarySlot::MakeEmpty(this));
	Slots.Add(UVMSummarySlot::MakeEmpty(this));
	Slots.Add(UVMSummarySlot::MakeEmpty(this));
	Slots.Add(UVMSummarySlot::MakeEmpty(this));
	Slots.Add(UVMSummarySlot::MakeEmpty(this));
}

/*static*/ UVMSummarySlot* UVMSummarySlot::MakeItem(UObject* Outer, const UHeroItemStack* Stack)
{
	if (!IsValid(Stack))
	{
		return UVMSummarySlot::MakeEmpty(Outer);
	}

	UVMSummarySlot* VM = CreateLordVM<UVMSummarySlot>(Outer);
	auto Def = Stack->GetItemDef();

	VM->SetIcon(Def->GetItemIcon());
	if (Def->GetCanStack())
	{
		VM->SetCount(Stack->GetCount());
	}

	return VM;
}

/*static*/ UVMSummarySlot* UVMSummarySlot::MakeUnitTypeCount(UObject* Outer, UUnitType* Type)
{
	UVMSummarySlot* VM = CreateLordVM<UVMSummarySlot>(Outer);
	
	VM->SetIcon(Type->UnitIcon);
	VM->SetMaxCount(3); // Put SOMETHING there so it shows up, and prompts
						// us to fix the wrong number if it's wrong

	return VM;
}

/*static*/ UVMSummarySlot* UVMSummarySlot::MakeVisitorCount(UObject* Outer)
{
	UVMSummarySlot* VM = CreateLordVM<UVMSummarySlot>(Outer);

	auto Assets = UWidgetBlueprintClassRegistry::Get();
	if (ensure(Assets))
	{
		VM->SetIcon(Assets->VisitorIcon.LoadSynchronous());
	}

	// Would need to reference the visitor texture?
	//SetIcon(UStaticAssets::GetVisitorIcon());

	return VM;
}
