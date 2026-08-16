// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "GameplayEffectVM.generated.h"

class UVisibleGameplayEffect;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMGameplayEffect : public UVMLordBase
{
    GENERATED_BODY()

public:
	UVMGameplayEffect();

	void Setup(const UVisibleGameplayEffect* Effect);

	FText GetName() const { return Name; }
	void SetName(FText InName) { UE_MVVM_SET_PROPERTY_VALUE(Name, InName); }

	FText GetDescription() const { return Description; }
	void SetDescription(FText InDescription) { UE_MVVM_SET_PROPERTY_VALUE(Description, InDescription); }

	UTexture2D* GetIcon() const { return Icon; }
	void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }


protected:
	UPROPERTY(FieldNotify, Getter, BlueprintReadOnly, Category = "Visible Effect")
	FText Name;

	UPROPERTY(FieldNotify, Getter, BlueprintReadOnly, Category = "Visible Effect")
	FText Description;

	UPROPERTY(FieldNotify, Getter, BlueprintReadOnly, Category = "Visible Effect")
	TObjectPtr<UTexture2D> Icon;
};
