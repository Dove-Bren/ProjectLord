// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "Gameplay/GameTeam.h"

#include "UnitViewModel.generated.h"

class AUnit;
class UAbilitySystemComponent;
class UCombatAttributeSet;

UCLASS()
class PROJECTLORD_API UVMUnit : public UVMLordBase
{
	GENERATED_BODY()

public:
	int GetMaxHealth() const { return MaxHealth; }
	int GetHealth() const { return Health; }

	EGameTeam GetTeam() const { return Team; }

private:
	static UVMUnit* CreateForUnit(AUnit* Unit);

	virtual void InitializeAttributeListeners(UAbilitySystemComponent* Component, UCombatAttributeSet* Attributes);

	// The unit that this view model represents
	UPROPERTY()
	TObjectPtr<const AUnit> Model;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Health", meta = (AllowPrivateAccess = true))
	int MaxHealth;
	void SetMaxHealth(int InMaxHealth) { UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, InMaxHealth); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Health", meta = (AllowPrivateAccess = true))
	int Health;
	void SetHealth(int InHealth) { UE_MVVM_SET_PROPERTY_VALUE(Health, InHealth); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Team", meta = (AllowPrivateAccess = true))
	EGameTeam Team;
	void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

	friend class AUnit;
};