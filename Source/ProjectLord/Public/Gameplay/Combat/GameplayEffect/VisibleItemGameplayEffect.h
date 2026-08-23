// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Combat/GameplayEffect/VisibleGameplayEffect.h"

#include "VisibleItemGameplayEffect.generated.h"

class UHeroItemDef;

UCLASS(Blueprintable, HideCategories = (VisibleEffect))
class PROJECTLORD_API UVisibleItemGameplayEffect : public UVisibleGameplayEffect
{
    GENERATED_BODY()

public:
    
    virtual void PostInitProperties() override;

    UFUNCTION(BlueprintPure)
    const UHeroItemDef* GetItemDef() const { return ItemDef; }

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Effect")
    TObjectPtr<const UHeroItemDef> ItemDef;
};
