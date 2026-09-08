// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Buildings/ResidentsViewModel.h"

void UVMResidents::SetResidents(TArray<UVMUnit*> InResidents)
{
	ensure(CanHaveResidents() || InResidents.IsEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(Residents, InResidents);
}
