// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroEquipment.h"

bool UHeroEquipmentDef::CanUse(UUnitType* HeroType) const
{
	if (!AllowedHeroTypes.Contains(HeroType))
	{
		return false;
	}

	return Super::CanUse(HeroType);
}

void UHeroItemStack::Init(UHeroItemDef* InItemDef, int InCount)
{
	check(IsValid(InItemDef));
	ItemDef = InItemDef;
	SetCount(InCount);
}

UHeroInventory::UHeroInventory()
{
	
}

void UHeroInventory::InitInventory(UHeroEquipmentDef* StarterWeapon, UHeroEquipmentDef* StarterArmor)
{
	if (ensure(StarterWeapon))
	{
		SetWeapon(UHeroItemStack::Make(this, StarterWeapon, 1));
	}
	if (ensure(StarterArmor))
	{
		SetArmor(UHeroItemStack::Make(this, StarterArmor, 1));
	}
}

UHeroItemStack* UHeroInventory::FindItem(const UHeroItemDef* ItemDef) const
{
	if (IsValid(Weapon) && ItemDef == Weapon->GetItemDef())
	{
		return Weapon;
	}
	if (IsValid(Armor) && ItemDef == Armor->GetItemDef())
	{
		return Armor;
	}
	for (auto InSlot : ExtraSlots)
	{
		if (InSlot->GetItemDef() == ItemDef)
		{
			return InSlot;
		}
	}

	return nullptr;
}

void UHeroInventory::SetWeapon(UHeroItemStack* InWeapon)
{
	if (InWeapon != Weapon)
	{
		Weapon = InWeapon;
		OnInventoryItemsChanged.Broadcast();
	}
}

void UHeroInventory::SetArmor(UHeroItemStack* InArmor)
{
	if (InArmor != Armor)
	{
		Armor = InArmor;
		OnInventoryItemsChanged.Broadcast();
	}
}

void UHeroInventory::SetHealthPotions(UHeroItemStack* InHealthPotions)
{
	HealthPotions = InHealthPotions;
}

void UHeroInventory::DeductHealthPotion()
{
	if (ensure(HealthPotions && HealthPotions->GetCount() > 0))
	{
		HealthPotions->SetCount(HealthPotions->GetCount() - 1);
	}
}

void UHeroInventory::SetManaPotions(UHeroItemStack* InManaPotions)
{
	ManaPotions = InManaPotions;
}

void UHeroInventory::DeductManaPotion()
{
	if (ensure(ManaPotions && ManaPotions->GetCount() > 0))
	{
		ManaPotions->SetCount(ManaPotions->GetCount() - 1);
	}
}

int UHeroInventory::AddPersonalGold(int Amount)
{
	PersonalGold = FMath::Max(0, PersonalGold + Amount);
	return PersonalGold;
}

int UHeroInventory::AddGuildGold(int Amount)
{
	GuildGold = FMath::Max(0, GuildGold + Amount);
	return GuildGold;
}

bool UHeroInventory::Add(UHeroItemStack* Item)
{
	if (!ensure(Item))
	{
		return false;
	}

	switch (Item->GetItemDef()->GetItemType())
	{
	case EItemType::Weapon:
		SetWeapon(Item);
		return true;
	case EItemType::Armor:
		SetArmor(Item);
		return true;
	case EItemType::HealthPotion:
		SetHealthPotions(Item);
		return true;
	case EItemType::ManaPotion:
		SetManaPotions(Item);
		return true;
	case EItemType::Other:
		return AddExtraItem(Item);
	}

	return false;
}

bool UHeroInventory::AddExtraItem(UHeroItemStack* ExtraItem)
{
	// Look for existing stack first
	if (auto InSlot = FindItem(ExtraItem->GetItemDef()))
	{
		// If it stacks, add to count. Otherwise, no dupes!
		if (InSlot->GetItemDef()->GetCanStack())
		{
			InSlot->AddCount(ExtraItem->GetCount());
			ExtraItem->AddCount(-ExtraItem->GetCount()); // Invalidate it
			return true;
		}
		// else
		return false;
	}

	if (ExtraSlots.Num() >= UHeroInventory::MaxExtraSlots)
	{
		return false;
	}

	ExtraSlots.Add(ExtraItem);
	OnInventoryItemsChanged.Broadcast();
	return true;
}

/*static*/ UHeroItemStack* UHeroInventory::MakeThrowawayStack(UObject* Outer, UHeroItemDef* Def, int Count)
{
	UHeroItemStack* Stack = NewObject<UHeroItemStack>(Outer);
	Stack->Init(Def, 1);
	return Stack;
}
