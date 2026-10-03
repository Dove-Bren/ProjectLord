// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/GameGood.h"

#include "Gameplay/Units/HeroEquipment.h"
#include "UI/ViewModels/GameGoodViewModel.h"
#include "UI/ViewModels/Generic/StatModifiersViewModel.h"

UVMGameGood* UGameGood::GetOrCreateViewModel()
{
	if (!ViewModel)
	{
		ViewModel = CreateLordVM<UVMGameGood>(this);
		ViewModel->SetName(GetName());
		ViewModel->SetDescription(GetDescription());
		ViewModel->SetIcon(GetIcon());

		if (GetAbility())
		{
			ViewModel->SetModifiers(UVMStatModifiers::Make(this, GetAbility().GetDefaultObject()->GetAbilityModifierPreview()));
		}
		else if (GetItemDef())
		{
			ViewModel->SetModifiers(UVMStatModifiers::Make(this, GetItemDef()->GetPreviewModifiers()));
		}
	}
	return ViewModel;
}

#if WITH_EDITOR
void UGameGood::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, Item) && Item != nullptr)
	{
		Name = Item->GetItemName();
		Description = Item->GetItemDescription();
		Icon = Item->GetItemIcon();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, Ability) && Ability != nullptr)
	{
		auto CDO = Ability.GetDefaultObject();
		Name = CDO->GetAbilityName();
		Description = CDO->GetAbilityDescription();
		Icon = CDO->GetAbilityIcon();
	}
}
#endif // WITH_EDITOR 
