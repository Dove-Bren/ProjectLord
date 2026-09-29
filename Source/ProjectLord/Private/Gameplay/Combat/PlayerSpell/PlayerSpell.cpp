// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/PlayerSpell/PlayerSpell.h"

#include "AbilitySystemComponent.h"

#include "Gameplay/LordPlayerController.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpellGameplayAbility.h"
#include "Gameplay/Buildings/Building.h"
#include "UI/ViewModels/PlayerSpellViewModel.h"

UPlayerSpell::UPlayerSpell()
{
	TargetType = ESpellTargetType::Enemy;
}

void UPlayerSpell::PostInitProperties()
{
	Super::PostInitProperties();
}

UVMPlayerSpell* UPlayerSpell::MakeViewModel(const UObject* WorldContextObject)
{
	auto ViewModel = CreateLordVM<UVMPlayerSpell>(WorldContextObject->GetWorld());
	ViewModel->SetSpellName(GetSpellName());
	ViewModel->SetDescription(GetSpellDescription());
	ViewModel->SetIcon(GetSpellIcon());
	ViewModel->SetBaseCost(GetBaseGoldCost());
	ViewModel->SetSpellToCast(this);
	return ViewModel;
}

bool UPlayerSpell::CanCastOn(const ALordPlayerController* Caster, UCombatComponent* Target) const
{
	// Might work on buildings, might not
	if (!bCanTargetBuildings && Cast<ABuilding>(Target->GetOwner()))
	{
		return false;
	}

	// Check team
	const bool Ally = ESpellTargetType::Ally == GetTargetType();
	if (Ally)
	{
		return Target->GetTeam() == Caster->GetTeam();
	}
	else
	{
		return Target->GetTeam() != Caster->GetTeam();
	}
}

bool UPlayerSpell::HasValidTarget(const ALordPlayerController* Caster) const
{
	// depends on target type
	if (ESpellTargetType::Ground == GetTargetType())
	{
		return CanCastAt(Caster, Caster->GetWorldPositionUnderMouse());
	}

	// else needs a combat target
	auto Target = Caster->GetCombatUnderMouse();
	if (!Target)
	{
		return false;
	}

	return CanCastOn(Caster, Target);
	
}

bool UPlayerSpell::CheckCost(const ALordPlayerController* Source, FVector At) const
{
	const auto Cost = GetGoldCostAt(Source, At);
	
	if (Cost > 0)
	{
		auto TeamState = Source->GetTeamState();
		if (!TeamState)
		{
			return false;
		}

		if (TeamState->GetGold() < Cost)
		{
			return false;
		}
	}

	return true;
}

void UPlayerSpell::ApplyCost(ALordPlayerController* Source, FVector At) const
{
	const auto Cost = GetGoldCostAt(Source, At);

	if (Cost > 0)
	{
		auto TeamState = Source->GetTeamState();
		if (ensure(TeamState))
		{
			TeamState->AddGold(-Cost);
		}
	}
}

bool UPlayerSpell::CanCast(const ALordPlayerController* Source, FVector At, UCombatComponent* Target) const
{
	if (!Source)
	{
		return false;
	}

	// Check ability
	// SNIP nevermind; we don't grant ability, so can't check.
	// Instead, we'll just accept that we may return true while there's a cooldown in effect
	/*if (ensure(CastAbility))
	{
		
	}*/

	// Check target
	if (!HasValidTarget(Source))
	{
		return false;
	}

	if (!CheckCost(Source, At))
	{
		return false;
	}

	return true;
}

bool UPlayerSpell::AttemptCast(ALordPlayerController* Source, FVector At, UCombatComponent* Target)
{
	if (!Source)
	{
		return false;
	}

	if (!HasValidTarget(Source)) // might be double checking with this
	{
		return false;
	}

	// Check ability
	if (ensure(CastAbility))
	{
		auto ASC = Source->GetAbilitySystemComponent();
		if (!ensure(ASC))
		{
			return false;
		}

		FGameplayAbilitySpec AbilitySpec(CastAbility);
		FGameplayEventData Payload = UPlayerSpellGameplayAbility::MakeEventPayload(Source, At, Target, this);
		auto Handle = ASC->GiveAbilityAndActivateOnce(AbilitySpec, &Payload);
		if (!Handle.IsValid())
		{
			return false;
		}
//////////////// From this point on, no returning false
	}

	ApplyCost(Source, At);

	return true;
}

#if WITH_EDITOR
void UPlayerSpell::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, CastAbility))
	{
		// Extract activation tag from ability
		FGameplayTag Tag = ULordGameplayTags::AbilityTriggerInvalid();
		if (CastAbility)
		{
			for (const auto& Trigger : CastAbility.GetDefaultObject()->GetAbilityTriggers())
			{
				if (Trigger.TriggerSource == EGameplayAbilityTriggerSource::GameplayEvent)
				{
					Tag = Trigger.TriggerTag;
					break;
				}
			}
		}

		TriggerTag = Tag;
	}
}
#endif // WITH_EDITOR
