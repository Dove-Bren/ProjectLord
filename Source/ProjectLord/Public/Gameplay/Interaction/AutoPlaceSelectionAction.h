// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Interaction/SelectionAction.h"

#include "AutoPlaceSelectionAction.generated.h"

UCLASS(Blueprintable)
class PROJECTLORD_API UAutoPlaceSelectionAction : public UPlaceBuildingPurchase
{
    GENERATED_BODY()

public:

    UAutoPlaceSelectionAction();

    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;

    virtual bool Perform_Implementation(const FSelectionActionContext& Context) const override;

};
