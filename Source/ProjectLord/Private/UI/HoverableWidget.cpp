// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/HoverableWidget.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/LordPlayerController.h"
#include "UI/ViewModels/SelectionViewModel.h"

FStaticSelection UHoverableWidget::GetHoverInfo_Implementation()
{
	ensureMsgf(false, TEXT("Selection info not implemented"));
	
	FStaticSelection BadSelection;
	BadSelection.Name = FText::FromString(TEXT("MISSING NAME"));
	BadSelection.Description = FText::FromString(TEXT("This widget said it wanted a hover selection, but didn't provide what should be shown."));
	return BadSelection;
}

void UHoverableWidget::PushHoverInfoToController()
{
	auto Controller = GetOwningPlayer<ALordPlayerController>();
	if (ensure(Controller))
	{
		Controller->SetHoveredStaticElement(GetHoverInfo());
	}
}

void UHoverableWidget::ClearHoverInfoFromController()
{
	auto Controller = GetOwningPlayer<ALordPlayerController>();
	if (ensure(Controller))
	{
		Controller->SetHovered(nullptr);
	}
}

void UHoverableWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (CanHover())
	{
		PushHoverInfoToController();
	}
}

void UHoverableWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (CanHover())
	{
		ClearHoverInfoFromController();
	}
}
