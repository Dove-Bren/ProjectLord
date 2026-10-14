// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/HeroInventory.h"

#include "Net/UnrealNetwork.h"

#include "UI/ViewModels/HeroInventoryViewModel.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"

UHeroInventory::UHeroInventory()
{
	GuildGold = 0;
	PersonalGold = 0;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UHeroInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Weapon);
	DOREPLIFETIME(ThisClass, Armor);
	DOREPLIFETIME(ThisClass, HealthPotions);
	DOREPLIFETIME(ThisClass, ManaPotions);
	DOREPLIFETIME(ThisClass, GuildGold);
	DOREPLIFETIME(ThisClass, PersonalGold);
	DOREPLIFETIME(ThisClass, ExtraSlots);
}

void UHeroInventory::InitInventory(UHeroEquipmentDef* StarterWeapon, UHeroEquipmentDef* StarterArmor)
{
	ViewModel = CreateLordVM<UVMHeroInventory>(this);
	auto GoldVM = CreateLordVM<UVMGold>(this);
	ViewModel->SetGoldVM(GoldVM);
	GoldVM->SetGold(GetPersonalGold());
	GoldVM->SetTaxGold(GetGuildGold());

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
		if (Weapon)
		{
			RemoveReplicatedSubObject(Weapon);
		}

		Weapon = InWeapon;
		AddReplicatedSubObject(Weapon);

		ViewModel->SetWeapon(Weapon->MakeViewModel());
		OnRep_Item();
	}
}

void UHeroInventory::SetArmor(UHeroItemStack* InArmor)
{
	if (InArmor != Armor)
	{
		if (Armor)
		{
			RemoveReplicatedSubObject(Armor);
		}

		Armor = InArmor;
		AddReplicatedSubObject(Armor);
		ViewModel->SetArmor(Armor->MakeViewModel());
		OnRep_Item();
	}
}

void UHeroInventory::SetHealthPotions(UHeroItemStack* InHealthPotions)
{
	if (HealthPotions)
	{
		RemoveReplicatedSubObject(HealthPotions);
	}

	HealthPotions = InHealthPotions;
	AddReplicatedSubObject(HealthPotions);

	ViewModel->SetHealthPotion(InHealthPotions->MakeViewModel());
	OnRep_Item();
}

void UHeroInventory::DeductHealthPotion()
{
	if (ensure(HealthPotions && HealthPotions->GetCount() > 0))
	{
		HealthPotions->SetCount(HealthPotions->GetCount() - 1);
		OnRep_Item();
	}
}

void UHeroInventory::SetManaPotions(UHeroItemStack* InManaPotions)
{
	ManaPotions = InManaPotions;
	ViewModel->SetManaPotion(InManaPotions->MakeViewModel());
	OnRep_Item();
}

void UHeroInventory::DeductManaPotion()
{
	if (ensure(ManaPotions && ManaPotions->GetCount() > 0))
	{
		ManaPotions->SetCount(ManaPotions->GetCount() - 1);
		OnRep_Item();
	}
}

int UHeroInventory::AddPersonalGold(int Amount)
{
	PersonalGold = FMath::Max(0, PersonalGold + Amount);
	ViewModel->GetGoldVM()->SetGold(GetPersonalGold());
	OnRep_Gold();
	return PersonalGold;
}

int UHeroInventory::AddGuildGold(int Amount)
{
	GuildGold = FMath::Max(0, GuildGold + Amount);
	ViewModel->GetGoldVM()->SetTaxGold(GetGuildGold());
	OnRep_Gold();
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
			OnRep_Item();
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
			OnRep_Item();
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
		{
			TArray<UVMItemStack*> ExtraItemVMs;
			for (auto Stack : ExtraSlots)
			{
				if (Stack && !Stack->IsEmpty())
				{
					ExtraItemVMs.Add(Stack->MakeViewModel());
				}
			}
			ViewModel->SetExtraItems(ExtraItemVMs);
		}
		OnRep_Item();
	}
	return true;
}

bool UHeroInventory::CanFit(UHeroItemStack* Item)
{
	return AddExtraItem(Item, true);
}

void UHeroInventory::OnRep_Item()
{
	OnInventoryItemsChanged.Broadcast();
}

void UHeroInventory::OnRep_Gold()
{
	OnInventoryGoldChanged.Broadcast();
}

