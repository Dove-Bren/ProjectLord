// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroEquipment.h"

UHeroItemDef::UHeroItemDef()
{
	
}

void UHeroItemStack::Init(UHeroItemDef* InItemDef, int InCount)
{
	check(IsValid(InItemDef));
	ItemDef = InItemDef;
	SetCount(InCount);
}

UHeroInventory::UHeroInventory()
{
	Weapon = EEquipmentTier::Starter;
	Armor = EEquipmentTier::Starter;
}

void UHeroInventory::InitInventory(FHeroEquipmentMap InEquipmentDefMap)
{
	EquipmentDefMap = InEquipmentDefMap;
}

UHeroItemStack* UHeroInventory::GetWeaponAsStack() const
{
	auto Def = EquipmentDefMap.GetWeaponForTier(Weapon);
	return MakeThrowawayStack(GetWorld(), Def, 1);
}

UHeroItemStack* UHeroInventory::GetArmorAsStack() const
{
	auto Def = EquipmentDefMap.GetArmorForTier(Armor);
	return MakeThrowawayStack(GetWorld(), Def, 1);
}

UHeroItemStack* UHeroInventory::GetHealthPotionsAsStack() const
{
	auto Def = EquipmentDefMap.HealthPotion;
	return MakeThrowawayStack(GetWorld(), Def, 1);
}

UHeroItemStack* UHeroInventory::GetManaPotionsAsStack() const
{
	auto Def = EquipmentDefMap.ManaPotion;
	return MakeThrowawayStack(GetWorld(), Def, 1);
}

UHeroItemStack* UHeroInventory::FindItem(const UHeroItemDef* ItemDef) const
{
	for (auto InSlot : ExtraSlots)
	{
		if (InSlot->GetItemDef() == ItemDef)
		{
			return InSlot;
		}
	}

	return nullptr;
}

void UHeroInventory::SetWeaponTier(EEquipmentTier WeaponTier)
{
	if (WeaponTier != Weapon)
	{
		Weapon = WeaponTier;
		OnInventoryItemsChanged.Broadcast();
	}
}

void UHeroInventory::SetArmorTier(EEquipmentTier ArmorTier)
{
	if (ArmorTier != Armor)
	{
		Armor = ArmorTier;
		OnInventoryItemsChanged.Broadcast();
	}
}

void UHeroInventory::AddHealthPotions(int Count)
{
	ensure(Count > 0);
	HealthPotions += Count;
}

void UHeroInventory::DeductHealthPotion()
{
	ensure(HealthPotions > 0);
	HealthPotions--;
}

void UHeroInventory::AddManaPotions(int Count)
{
	ensure(Count > 0);
	ManaPotions += Count;
}

void UHeroInventory::DeductManaPotion()
{
	ensure(ManaPotions > 0);
	ManaPotions--;
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
