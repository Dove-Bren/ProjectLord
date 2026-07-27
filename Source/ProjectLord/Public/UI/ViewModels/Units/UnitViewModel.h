// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/ViewModels/LordViewModelBase.h"

#include "UnitViewModel.generated.h"

class AUnit;

UCLASS()
class PROJECTLORD_API UVMUnit : public UVMLordBase
{
	GENERATED_BODY()

public:
	int GetMaxHealth() const { return MaxHealth; }
	int GetHealth() const { return Health; }

private:
	// The unit that this view model represents
	UPROPERTY()
	TObjectPtr<AUnit> Model;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Health", meta = (AllowPrivateAccess = true))
	int MaxHealth;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Health", meta = (AllowPrivateAccess = true))
	int Health;

	friend class AUnit;
};