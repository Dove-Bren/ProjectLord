// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffect.h"

#include "GenericGameplayTagEffect.generated.h"

class UTargetTagsGameplayEffectComponent;

UCLASS(BlueprintType)
class PROJECTLORD_API UGEGenericGameplayTag : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UGEGenericGameplayTag();

    void AddTag(FGameplayTag Tag);

protected:
    TObjectPtr<UTargetTagsGameplayEffectComponent> TagContainer;
};
