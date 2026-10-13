// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffect.h"

#include "HeroLevelGameplayEffect.generated.h"

UCLASS(Abstract)
class PROJECTLORD_API UGEHeroLevel : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UGEHeroLevel();

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR  

protected:

    // Attributes to add for every level the unit gains.
    // Typically, just set to 1 (or so) damage of the type relevant to the class.
    UPROPERTY(EditDefaultsOnly, Category = "Growth", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.AttributeBaseValue"))
    TObjectPtr<UDataTable> ClassAttributeGrowth;

    void RefreshModifiers();
};
