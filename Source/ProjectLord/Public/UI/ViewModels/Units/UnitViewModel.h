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

	FText GetName() const { return Name; }
	void SetName(FText InName) { UE_MVVM_SET_PROPERTY_VALUE(Name, InName); }

	FText GetDescription() const { return Description; }
	void SetDescription(FText InDescription) { UE_MVVM_SET_PROPERTY_VALUE(Description, InDescription); }

	EGameTeam GetTeam() const { return Team; }
	UTexture2D* GetIcon() const { return Icon; }

	UVMCombatData* GetCombatVM() const { return CombatVM; }
	UVMAction* GetActionVM() const { return ActionVM; }

	bool IsSelected() const { return bIsSelected; }
	void SetIsSelected(bool bInIsSelected) { UE_MVVM_SET_PROPERTY_VALUE(bIsSelected, bInIsSelected); }

	UFUNCTION(BlueprintCallable, Category = "Unit")
	void SelectUnit();

protected:
	static UVMUnit* CreateForUnit(AUnit* Unit);

	// The unit that this view model represents
	UPROPERTY()
	TObjectPtr<const AUnit> Model;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "SubVM")
	TObjectPtr<UVMCombatData> CombatVM;
	void SetCombatVM(UVMCombatData* VM) { UE_MVVM_SET_PROPERTY_VALUE(CombatVM, VM); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "SubVM")
	TObjectPtr<UVMAction> ActionVM;
	void SetActionVM(UVMAction* VM) { UE_MVVM_SET_PROPERTY_VALUE(ActionVM, VM); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Unit")
	FText Name;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Unit")
	FText Description;

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Team")
	EGameTeam Team;
	void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Icon")
	TObjectPtr<UTexture2D> Icon;
	void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

	UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = IsSelected, Category = "Selection")
	bool bIsSelected;

	friend class AUnit;
};