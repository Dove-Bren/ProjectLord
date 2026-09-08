// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Generic/InspectableViewModel.h"

#include "Kismet/GameplayStatics.h"
#include "Gameplay/LordPlayerController.h"
#include "UI/InspectWidget.h"

/*static*/ UVMInspectable* UVMInspectable::Make(UObject* Outer, TSubclassOf<UInspectWidget> InspectWidgetClass, FGetInspectVM VMGetter)
{
	return Make(Outer, FOnInspect::CreateWeakLambda(Outer, [Outer, InspectWidgetClass, VMGetter]()
	{
		if (auto PC = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(Outer, 0)))
		{
			PC->ShowInspectWidget(InspectWidgetClass, VMGetter.Execute());
		}
	}));
}

void UVMInspectable::Inspect()
{
	if (ensure(InspectAction.IsBound()))
	{
		InspectAction.Execute();
	}
}
