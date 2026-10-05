// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Interaction/SelectionAction.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/GameTeam.h"
#include "Gameplay/GameTeamState.h"
#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Interaction/SelectionComponent.h"
#include "Gameplay/Units/RewardFlag.h"
#include "Gameplay/Units/Unit.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/BuildingTypes.h"
#include "Gameplay/Buildings/GoodBuilding.h"
#include "Gameplay/Buildings/BuildingActionQueue.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpell.h"
#include "UI/ViewModels/SelectionActionViewModel.h"

UVMSelectionAction* USelectionAction::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = ConstructBaseVM(Outer);

	// Automatically set up first enabled/hidden state so that
	// subclasses just need to register listeners for changes

	ESelectionActionFailureReason Reason;
	const bool bEnabled = CanPerform(InContext, Reason);
	const bool bHidden = IsHidden(InContext);
	ViewModel->SetEnabled(bEnabled, Reason);
	ViewModel->SetHidden(bHidden);

	return ViewModel;
}

UVMSelectionAction* USelectionAction::ConstructBaseVM(UObject* Outer) const
{
	auto VM = CreateLordVM<UVMSelectionAction>(Outer);
	VM->Init(this);
	return VM;
}

bool USelectionAction::IsHidden_Implementation(const FSelectionActionContext& InContext) const
{
	return false;
}

bool USelectionAction::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	ensureMsgf(false, TEXT("Selection Action did not implement CanPerform"));
	ReasonOut = ESelectionActionFailureReason::None;
	return false;
}

bool USelectionAction::Perform_Implementation(const FSelectionActionContext& InContext) const
{
	ensureMsgf(false, TEXT("Selection Action did not implement Perform"));
	return false;
}

UVMSelectionAction* USelectionPurchase::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = Super::MakeViewModel_Implementation(Outer,  InContext);

	if (InContext.TeamState)
	{
		InContext.TeamState->OnTeamGoldChanged.AddWeakLambda(this, [this, InContext, ViewModel](int Gold)
			{
				ESelectionActionFailureReason Reason;
				bool bEnabled = CanPerform(InContext, Reason);
				ViewModel->SetEnabled(bEnabled, Reason);
			});
	}

	return ViewModel;
}

bool USelectionPurchase::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	if (!InContext.TeamState)
	{
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	if (InContext.TeamState->GetGold() < GetGoldCost())
	{
		ReasonOut = ESelectionActionFailureReason::CantAfford;
		return false;
	}
	
	return true;
}

bool USelectionPurchase::DeductGoldCost(const FSelectionActionContext& InContext) const
{
	const int Cost = GetGoldCost();
	if (InContext.TeamState && InContext.TeamState->GetGold() >= Cost)
	{
		InContext.TeamState->AddGold(-Cost);
		return true;
	}
	return false;
}

//void USelectionPurchase::SetGoldCost(int InCost)
//{
//	if (GoldCost != InCost)
//	{
//		GoldCost = InCost;
//		ViewModel->SetGoldCost(InCost);
//		{
//			ESelectionActionFailureReason Reason;
//			bool bEnabled = CanPerform(Reason);
//			ViewModel->SetEnabled(bEnabled, Reason);
//		}
//	}
//} MOVE dynamic cost to viewmodel ?

AUnit* UUnitBasedPurchase::GetUnit(const FSelectionActionContext& InContext) const
{
	return Cast<AUnit>(InContext.Selection->GetOwner());
}

UVMSelectionAction* UUnitBasedPurchase::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = Super::MakeViewModel_Implementation(Outer,  InContext);

	//AUnit* UnitOwner = GetUnitInner(); // ? This was doing nothing

	return ViewModel;
}

bool UUnitBasedPurchase::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	// Must have a unit performing us
	AUnit* UnitOwner = GetUnit(InContext);
	if (!IsValid(UnitOwner))
	{
		// Not sure what to put here...
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	return Super::CanPerform_Implementation(InContext, ReasonOut);
}

ARewardFlag* UFlagBasedPurchase::GetFlag(const FSelectionActionContext& InContext) const
{
	return Cast<ARewardFlag>(InContext.Selection->GetOwner());
}

UVMSelectionAction* UFlagBasedPurchase::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = Super::MakeViewModel_Implementation(Outer,  InContext);

	ARewardFlag* FlagOwner = GetFlag(InContext);
	//TODO:
	// FlagOwner->OnDeath.AddWeakLambda()...

	return ViewModel;
}

bool UFlagBasedPurchase::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	// Must have a unit performing us
	ARewardFlag* FlagOwner = GetFlag(InContext);
	if (!IsValid(FlagOwner))
	{
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	return Super::CanPerform_Implementation(InContext, ReasonOut);
}

ABuilding* UBuildingBasedPurchase::GetBuilding(const FSelectionActionContext& InContext) const
{
	auto ActionOwner = InContext.Selection->GetOwner();
	return Cast<ABuilding>(ActionOwner);
}

UVMSelectionAction* UBuildingBasedPurchase::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = Super::MakeViewModel_Implementation(Outer,  InContext);

	auto BuildingOwner = GetBuilding(InContext);
	if (ensure(BuildingOwner))
	{
		// TODO building destruction
		BuildingOwner->OnBuildingLevelChanged.AddWeakLambda(this, [this, InContext, ViewModel](int NewLevel) {
			ESelectionActionFailureReason Reason;
			bool bEnabled = CanPerform(InContext, Reason);
			ViewModel->SetEnabled(bEnabled, Reason);
		});
		BuildingOwner->OnBuildingHealthChanged.AddWeakLambda(this, [this, InContext, ViewModel](int Health, int MaxHealth) {
			ESelectionActionFailureReason Reason;
			bool bEnabled = CanPerform(InContext, Reason);
			ViewModel->SetEnabled(bEnabled, Reason);
			});
	}

	return ViewModel;
}

bool UBuildingBasedPurchase::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	// Must have a building performing us
	ABuilding* BuildingOwner = GetBuilding(InContext);
	if (!IsValid(BuildingOwner))
	{
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	// Check building level requirement
	if (BuildingOwner->GetBuildingLevel() < RequiredBuildingLevel)
	{
		ReasonOut = RequiredBuildingLevel == 1 ? ReasonOut = ESelectionActionFailureReason::BuildingInProgress
					: RequiredBuildingLevel == 2 ? ESelectionActionFailureReason::NeedLevel2
					: ReasonOut = ESelectionActionFailureReason::NeedLevel3;
		return false;
	}

	return Super::CanPerform_Implementation(InContext, ReasonOut);
}

void UResearchGoodPurchase::PostInitProperties()
{
	Super::PostInitProperties();

	// Get name, desc, icon from the linked good
	if (Good.Good)
	{
		Name = Good.Good->GetName();
		Description = Good.Good->GetDescription();
		Icon = Good.Good->GetIcon();
	}
}

UVMSelectionAction* UResearchGoodPurchase::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = Super::MakeViewModel_Implementation(Outer,  InContext);

	auto BuildingOwner = GetBuilding(InContext);
	auto GoodBuilding = Cast<AGoodBuilding>(BuildingOwner);
	if (ensure(GoodBuilding))
	{
		GoodBuilding->OnBuildingGoodsChanged.AddWeakLambda(this, [this, InContext, ViewModel]()
		{
			ViewModel->SetHidden(IsHidden(InContext));
		});
		GoodBuilding->OnBuildingQueueChanged.AddWeakLambda(this, [this, InContext, ViewModel]()
		{
			ESelectionActionFailureReason Reason;
			bool bEnabled = CanPerform(InContext, Reason);
			ViewModel->SetEnabled(bEnabled, Reason);
		});
	}

	return ViewModel;
}

bool UResearchGoodPurchase::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	if (!Super::CanPerform_Implementation(InContext, ReasonOut))
	{
		return false;
	}

	if (IsHidden(InContext))
	{
		return false;
	}

	// Check if building has good already
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding(InContext));
	if (!ensure(IsValid(BuildingOwner)))
	{
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	auto Queue = BuildingOwner->GetQueueComponent();

	if (Queue->IsFull())
	{
		ReasonOut = ESelectionActionFailureReason::QueueFull;
		return false;
	}

	return true;
}

bool UResearchGoodPurchase::IsHidden_Implementation(const FSelectionActionContext& InContext) const
{
	// Check if building has good already
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding(InContext));
	if (!ensure(IsValid(BuildingOwner)))
	{
		return false;
	}

	return BuildingOwner->HasGood(GetGood().Good);
}

bool UResearchGoodPurchase::Perform_Implementation(const FSelectionActionContext& InContext) const
{
	AGoodBuilding* Building = Cast<AGoodBuilding>(GetBuilding(InContext));
	if (!ensure(Building))
	{
		return false;
	}
	auto Queue = Building->GetQueueComponent();

	auto Action = NewObject<UQueuedGoodAction>(Queue);
	Action->Init(GetGoldCost(), GetGood());

	DeductGoldCost(InContext);
	Queue->QueueAction(Action);

	return true;
}

void UResearchPlayerSpellPurchase::PostInitProperties()
{
	Super::PostInitProperties();

	// Get name, desc, icon from the linked good
	if (Spell)
	{
		Name = Spell->GetSpellName();
		Description = Spell->GetSpellDescription();
		Icon = Spell->GetSpellIcon();
	}
}

UVMSelectionAction* UResearchPlayerSpellPurchase::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = Super::MakeViewModel_Implementation(Outer,  InContext);

	auto BuildingOwner = GetBuilding(InContext);
	auto GoodBuilding = Cast<AGoodBuilding>(BuildingOwner);
	if (ensure(GoodBuilding))
	{
		GoodBuilding->OnBuildingSpellsChanged.AddWeakLambda(this, [this, InContext, ViewModel]()
			{
				ViewModel->SetHidden(IsHidden(InContext));
			});
		GoodBuilding->OnBuildingQueueChanged.AddWeakLambda(this, [this, InContext, ViewModel]()
			{
				ESelectionActionFailureReason Reason;
				bool bEnabled = CanPerform(InContext, Reason);
				ViewModel->SetEnabled(bEnabled, Reason);
			});
	}

	return ViewModel;
}

bool UResearchPlayerSpellPurchase::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	if (!Super::CanPerform_Implementation(InContext, ReasonOut))
	{
		return false;
	}

	if (IsHidden(InContext))
	{
		return false;
	}

	// Check if building has spell already
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding(InContext));
	if (!ensure(IsValid(BuildingOwner)))
	{
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	if (BuildingOwner->GetBuildingPlayerSpells().Contains(GetSpell()))
	{
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	auto Queue = BuildingOwner->GetQueueComponent();

	if (Queue->IsFull())
	{
		ReasonOut = ESelectionActionFailureReason::QueueFull;
		return false;
	}

	// Inspect queue
	for (auto Action : Queue->GetQueue())
	{
		if (auto SpellAction = Cast<UQueuedSpellAction>(Action))
		{
			if (SpellAction->GetSpell() == GetSpell())
			{
				return false;
			}
		}
	}

	return true;
}

bool UResearchPlayerSpellPurchase::IsHidden_Implementation(const FSelectionActionContext& InContext) const
{
	// Check if building has spell already
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding(InContext));
	if (!ensure(IsValid(BuildingOwner)))
	{
		return false;
	}

	return BuildingOwner->GetBuildingPlayerSpells().Contains(GetSpell());
}

bool UResearchPlayerSpellPurchase::Perform_Implementation(const FSelectionActionContext& InContext) const
{
	AGoodBuilding* Building = Cast<AGoodBuilding>(GetBuilding(InContext));
	if (!ensure(Building))
	{
		return false;
	}
	auto Queue = Building->GetQueueComponent();

	auto Action = NewObject<UQueuedSpellAction>(Queue);
	Action->Init(GetGoldCost(), GetSpell());

	DeductGoldCost(InContext);
	Queue->QueueAction(Action);

	return true;
}

void UPlaceBuildingPurchase::PostInitProperties()
{
	Super::PostInitProperties();

	// Get name, desc, icon from the building type
	if (IsValid(BuildingType))
	{
		Name = BuildingType->BuildingName;
		Description = BuildingType->BuildingDescription;
		Icon = BuildingType->BuildingIcon;
	}

	// Also capture base price, since we're going to adjust the normal cost
	BaseCost = GetGoldCost();
}

int UPlaceBuildingPurchase::CalculatePrice(const FSelectionActionContext& InContext) const
{
	// Adjust cost based on number of the same buildings on the team
	// CostExponentBase^(n) * Cost
	const int Count = InContext.TeamState
		? InContext.TeamState->GetTeamBuildingsOfType(GetBuildingType()).Num()
		: 0;
	const float RawCost = FMath::Pow(CostExponentBase, Count) * BaseCost;
	int AdjCost = FMath::FloorToInt(RawCost);
	AdjCost -= AdjCost % 10; // round down to nearest 10
	
	return AdjCost;
}

UVMSelectionAction* UPlaceBuildingPurchase::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = Super::MakeViewModel_Implementation(Outer,  InContext);

	// Override cost with cost based on number of buildings.
	auto AdjustCost = [this, InContext, ViewModel]() {
		const int RealCost = CalculatePrice(InContext);
		ViewModel->SetGoldCost(RealCost);
	};
	if (InContext.TeamState) InContext.TeamState->OnTeamBuildingsChanged.AddWeakLambda(this, AdjustCost);
	AdjustCost();

	return ViewModel;
}

bool UPlaceBuildingPurchase::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	// TODO: check building requirements

	if (!ensure(IsValid(BuildingType)))
	{
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	return Super::CanPerform_Implementation(InContext, ReasonOut);
}

bool UPlaceBuildingPurchase::Perform_Implementation(const FSelectionActionContext& InContext) const
{
	ALordPlayerController* Controller = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!Controller)
	{
		return false;
	}

	Controller->PlaceBuilding(GetBuildingType(), GetGoldCost());
	return true;
}

void URecruitUnitPurchase::PostInitProperties()
{
	Super::PostInitProperties();

	// Get name, desc, icon from the unit type
	auto Type = GetUnitType();
	if (IsValid(Type))
	{
		Name = Type->UnitName;
		Description = Type->UnitDescription;
		Icon = Type->UnitIcon;
	}
}

UVMSelectionAction* URecruitUnitPurchase::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = Super::MakeViewModel_Implementation(Outer,  InContext);

	auto ResidentOwner = Cast<AResidentialBuilding>(GetBuilding(InContext));
	if (ensure(ResidentOwner))
	{
		ResidentOwner->OnResidentsChanged.AddWeakLambda(this, [this, InContext, ViewModel]()
			{
				ESelectionActionFailureReason Reason;
				bool bEnabled = CanPerform(InContext, Reason);
				ViewModel->SetEnabled(bEnabled, Reason);
			});
		auto GoodOwner = Cast<AGoodBuilding>(ResidentOwner);
		if (ensure(GoodOwner))
		{
			GoodOwner->OnBuildingQueueChanged.AddWeakLambda(this, [this, InContext, ViewModel]()
				{
					ESelectionActionFailureReason Reason;
					bool bEnabled = CanPerform(InContext, Reason);
					ViewModel->SetEnabled(bEnabled, Reason);
				});
		}
	}

	return ViewModel;
}

bool URecruitUnitPurchase::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	AGoodBuilding* BuildingOwner = Cast<AGoodBuilding>(GetBuilding(InContext));
	if (!ensure(IsValid(BuildingOwner)))
	{
		ReasonOut = ESelectionActionFailureReason::None;
		return false;
	}

	if (!BuildingOwner->CanFitResidentType(GetUnitType()))
	{
		ReasonOut = ESelectionActionFailureReason::GuildFull;
		return false;
	}
	
	auto Queue = BuildingOwner->GetQueueComponent();

	if (Queue->IsFull())
	{
		ReasonOut = ESelectionActionFailureReason::QueueFull;
		return false;
	}

	return Super::CanPerform_Implementation(InContext, ReasonOut);
}

bool URecruitUnitPurchase::Perform_Implementation(const FSelectionActionContext& InContext) const
{
	AGoodBuilding* Building= Cast<AGoodBuilding>(GetBuilding(InContext));
	if (!ensure(Building))
	{
		return false;
	}

	auto Queue = Building->GetQueueComponent();

	auto Action = NewObject<UQueuedRecruitAction>(Queue);
	Action->Init(GetGoldCost(), GetUnitType());

	DeductGoldCost(InContext);
	Queue->QueueAction(Action);

	return true;
}
