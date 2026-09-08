// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/Units/UnitViewModel.h"
#include "HeroViewModel.generated.h"

class UVMCombatAbility;
class UVMHeroInventory;
class UVMLevel;
class AHeroBase;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMHero : public UVMUnit
{
	GENERATED_BODY()

public:

	static UVMHero* Make(AHeroBase* Hero);

	FText GetHeroName() const { return HeroName; }
	void SetHeroName(FText InName) { UE_MVVM_SET_PROPERTY_VALUE(HeroName, InName); }

	UVMHeroInventory* GetInventory() const { return Inventory; }
	void SetInventory(UVMHeroInventory* InInventory) { UE_MVVM_SET_PROPERTY_VALUE(Inventory, InInventory); }

	const TArray<UVMCombatAbility*>& GetAbilities() const { return Abilities; }
	void SetAbilities(TArray<UVMCombatAbility*> InAbilities) { UE_MVVM_SET_PROPERTY_VALUE(Abilities, InAbilities); }

	UVMLevel* GetLevelVM() const { return LevelVM; }
	void SetLevelVM(UVMLevel* VM) { UE_MVVM_SET_PROPERTY_VALUE(LevelVM, VM); }

	int GetStrength() const { return Strength; }
	void SetStrength(int InStrength) { UE_MVVM_SET_PROPERTY_VALUE(Strength, InStrength); }

	int GetAgility() const { return Agility; }
	void SetAgility(int InAgility) { UE_MVVM_SET_PROPERTY_VALUE(Agility, InAgility); }

	int GetIntelligence() const { return Intelligence; }
	void SetIntelligence(int InIntelligence) { UE_MVVM_SET_PROPERTY_VALUE(Intelligence, InIntelligence); }

	int GetStamina() const { return Stamina; }
	void SetStamina(int InStamina) { UE_MVVM_SET_PROPERTY_VALUE(Stamina, InStamina); }

	float GetMovement() const { return Movement; }
	void SetMovement(int InMovement) { UE_MVVM_SET_PROPERTY_VALUE(Movement, InMovement); }


protected:

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Hero")
	FText HeroName;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Hero")
	TObjectPtr<UVMHeroInventory> Inventory;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Hero")
	TObjectPtr<UVMLevel> LevelVM;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Hero")
	TArray<UVMCombatAbility*> Abilities;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Hero")
	float Movement;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Hero")
	int Strength;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Hero")
	int Agility;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Hero")
	int Intelligence;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Hero")
	int Stamina;

};
