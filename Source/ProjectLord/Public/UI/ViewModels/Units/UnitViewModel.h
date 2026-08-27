// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "Gameplay/GameTeam.h"

#include "UnitViewModel.generated.h"

class AUnit;
class UTexture2D;
class UVMCombatData;
class UVMAction;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMUnit : public UVMLordBase
{
	GENERATED_BODY()

public:


	EGameTeam GetTeam() const { return Team; }
	UTexture2D* GetIcon() const { return Icon; }

	UVMCombatData* GetCombatVM() const { return CombatVM; }
	UVMAction* GetActionVM() const { return ActionVM; }

	bool IsHero() const { return bIsHero; }
	void SetIsHero(bool bInIsHero) { UE_MVVM_SET_PROPERTY_VALUE(bIsHero, bInIsHero); }

private:
	static UVMUnit* CreateForUnit(AUnit* Unit);

	// The unit that this view model represents
	UPROPERTY()
	TObjectPtr<const AUnit> Model;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "SubVM", meta = (AllowPrivateAccess = true))
	TObjectPtr<UVMCombatData> CombatVM;
	void SetCombatVM(UVMCombatData* VM) { UE_MVVM_SET_PROPERTY_VALUE(CombatVM, VM); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "SubVM", meta = (AllowPrivateAccess = true))
	TObjectPtr<UVMAction> ActionVM;
	void SetActionVM(UVMAction* VM) { UE_MVVM_SET_PROPERTY_VALUE(ActionVM, VM); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Team", meta = (AllowPrivateAccess = true))
	EGameTeam Team;
	void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Icon", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> Icon;
	void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = IsHero, Category = "UnitType", meta = (AllowPrivateAccess = true))
	bool bIsHero;

	friend class AUnit;
};