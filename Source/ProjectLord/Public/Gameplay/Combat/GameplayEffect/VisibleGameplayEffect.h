// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffect.h"

#include "VisibleGameplayEffect.generated.h"

UCLASS(Blueprintable)
class PROJECTLORD_API UVisibleGameplayEffect : public UGameplayEffect
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	FText GetName() const { return EffectName; }

	UFUNCTION(BlueprintPure)
	FText GetDescription() const { return EffectDescription; }

	UFUNCTION(BlueprintPure)
	UTexture2D* GetIcon() const { return Icon; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visible Effect")
	FText EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visible Effect")
	FText EffectDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visible Effect")
	TObjectPtr<UTexture2D> Icon;
};
