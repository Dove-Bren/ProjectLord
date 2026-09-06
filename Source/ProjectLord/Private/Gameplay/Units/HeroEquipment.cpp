// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroEquipment.h"

void UHeroItemStack::Init(const UHeroItemDef* InItemDef, int InCount)
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
	OnInventoryItemsChanged.Broadcast();
}

void UHeroInventory::DeductHealthPotion()
{
	if (ensure(HealthPotions && HealthPotions->GetCount() > 0))
	{
		HealthPotions->SetCount(HealthPotions->GetCount() - 1);
		OnInventoryItemsChanged.Broadcast();
	}
}

void UHeroInventory::SetManaPotions(UHeroItemStack* InManaPotions)
{
	ManaPotions = InManaPotions;
	OnInventoryItemsChanged.Broadcast();
}

void UHeroInventory::DeductManaPotion()
{
	if (ensure(ManaPotions && ManaPotions->GetCount() > 0))
	{
		ManaPotions->SetCount(ManaPotions->GetCount() - 1);
		OnInventoryItemsChanged.Broadcast();
	}
}

int UHeroInventory::AddPersonalGold(int Amount)
{
	PersonalGold = FMath::Max(0, PersonalGold + Amount);
	OnInventoryGoldChanged.Broadcast();
	return PersonalGold;
}

int UHeroInventory::AddGuildGold(int Amount)
{
	GuildGold = FMath::Max(0, GuildGold + Amount);
	OnInventoryGoldChanged.Broadcast();
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
		if (!HealthPotions || HealthPotions->IsEmpty() || HealthPotions->GetItemDef() != Item->GetItemDef())
		{
			SetHealthPotions(Item);
		}
		else
		{
			HealthPotions->AddCount(Item->GetCount());
			OnInventoryItemsChanged.Broadcast();
		}
		return true;
	case EItemType::ManaPotion:
		if (!ManaPotions || ManaPotions->IsEmpty() || ManaPotions->GetItemDef() != Item->GetItemDef())
		{
			SetManaPotions(Item);
		}
		else
		{
			ManaPotions->AddCount(Item->GetCount());
			OnInventoryItemsChanged.Broadcast();
		}
		return true;
	case EItemType::Other:
		return AddExtraItem(Item);
	}

	return false;
}

bool UHeroInventory::AddExtraItem(UHeroItemStack* ExtraItem, bool bSimulateOnly)
{
	// Look for existing stack first
	if (auto InSlot = FindItem(ExtraItem->GetItemDef()))
	{
		// If it stacks, add to count. Otherwise, no dupes!
		if (InSlot->GetItemDef()->GetCanStack())
		{
			if (!bSimulateOnly)
			{
				InSlot->AddCount(ExtraItem->GetCount());
				ExtraItem->AddCount(-ExtraItem->GetCount()); // Invalidate it
			}
			return true;
		}
		// else
		return false;
	}

	if (ExtraSlots.Num() >= UHeroInventory::MaxExtraSlots)
	{
		return false;
	}

	if (!bSimulateOnly)
	{
		ExtraSlots.Add(ExtraItem);
		OnInventoryItemsChanged.Broadcast();
	}
	return true;
}

bool UHeroInventory::CanFit(UHeroItemStack* Item)
{
	return AddExtraItem(Item, true);
}

