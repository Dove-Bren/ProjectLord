// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/LordGameHUDWidget.h"

#include "UI/ToastNotification.h"

void ULordGameHUDWidget::AddToastNotification_Implementation(FToastNotification Notification)
{
	auto VM = Notification.MakeViewModel(this);
	PushToastNotification(VM);
}

void ULordGameHUDWidget::PushToastNotification_Implementation(UVMToast* Notification)
{

}
