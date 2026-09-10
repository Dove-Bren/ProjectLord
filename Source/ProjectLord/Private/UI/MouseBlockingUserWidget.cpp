// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/MouseBlockingUserWidget.h"

FReply UMouseBlockingUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return bBlockMouseClicks ? FReply::Handled() : FReply::Unhandled();
}
