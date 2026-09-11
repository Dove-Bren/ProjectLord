// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ToastNotification.h"
#include "UI/ViewModels/ToastViewModel.h"

UVMToast* FToastNotification::MakeViewModel(UObject* Outer)
{
	UVMToast* VM = CreateLordVM<UVMToast>(Outer);
	
	VM->SetType(Type);
	VM->SetContent(Content);
	VM->SetIcon(Icon);

	return VM;
}