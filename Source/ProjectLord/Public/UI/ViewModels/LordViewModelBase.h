// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MVVMViewModelBase.h"

#include "LordViewModelBase.generated.h"

template<typename T>
T* CreateLordVM(UObject* Outer)
{
	T* ViewModel = NewObject<T>(Outer, T::StaticClass());
	return ViewModel;
};

UCLASS()
class PROJECTLORD_API UVMLordBase : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	// Nothing of interest added just yet, but it is nice to have this base class in case we need it down the line.
};
