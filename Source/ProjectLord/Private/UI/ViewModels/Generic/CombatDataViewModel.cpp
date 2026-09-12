// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Generic/CombatDataViewModel.h"

#include "AbilitySystemComponent.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Combat/GameplayEffect/VisibleGameplayEffect.h"
#include "UI/ViewModels/GameplayEffectVM.h"

#include "Gameplay/Attributes/AttributeListener.h"

void UVMCombatData::Init(UCombatComponent* Component)
{
	// Register listeners and initial values
	auto ASC = Component->GetAbilitySubsystemComponent();
	auto AttributeSet = Component->GetCombatAttributeSet();

	REGISTER_ATTR_LISTENER_INT(Health);
	REGISTER_ATTR_LISTENER_INT(MaxHealth);

	REGISTER_ATTR_LISTENER_INT(Mana);
	REGISTER_ATTR_LISTENER_INT(MaxMana);

	REGISTER_ATTR_LISTENER_INT(MeleeDefense);
	REGISTER_ATTR_LISTENER_INT(RangedDefense);
	REGISTER_ATTR_LISTENER_INT(MagicDefense);

	REGISTER_ATTR_LISTENER_INT(MeleeDamage);
	REGISTER_ATTR_LISTENER_INT(RangedDamage);
	REGISTER_ATTR_LISTENER_INT(MagicDamage);

	REGISTER_ATTR_LISTENER_FLOAT(Sight);
	REGISTER_ATTR_LISTENER_FLOAT(AttackRange);

	// Listener for damage types handled by abilities listener
	SetDamageTypes(Component->GetRelevantDamageTypes());

	Component->OnTargetChange.AddDynamic(this, &ThisClass::OnTargetChange);
	SetTarget(Component->GetCombatTarget());

	Component->OnInvulnerabilityChange.AddDynamic(this, &ThisClass::OnInvulnerabilityChange);
	SetInvulnerable(Component->IsInvulnerable());

	Component->OnEffectsChange.AddDynamic(this, &ThisClass::OnEffectsChange);
	SetupEffects(Component->GetActiveVisibleEffects());

	Component->OnAbilitiesChange.AddDynamic(this, &ThisClass::OnAbilitiesChange);
	SetupAbilities(Component->GetCombatAbilities());
}

void UVMCombatData::OnEffectsChange(UCombatComponent* SelfComponent)
{
	SetupEffects(SelfComponent->GetActiveVisibleEffects());
}

void UVMCombatData::OnAbilitiesChange(UCombatComponent* Component)
{
	SetupAbilities(Component->GetCombatAbilities());

	// Also refresh damage types
	SetDamageTypes(Component->GetRelevantDamageTypes());
}

void UVMCombatData::SetupEffects(const TArray<const UVisibleGameplayEffect*>& InEffects)
{
	TArray<UVMGameplayEffect*> NewEffects;

	for (auto Effect : InEffects)
	{
		auto VM = CreateLordVM<UVMGameplayEffect>(this);
		VM->Setup(Effect);
		NewEffects.Add(VM);
	}

	SetEffects(NewEffects);
}

void UVMCombatData::SetupAbilities(TArray<UCombatAbility*> InAbilities)
{
	TArray<UVMCombatAbility*> NewAbilities;

	for (auto Ability : InAbilities)
	{
		NewAbilities.Add(Ability->GetOrCreateViewModel());
	}

	SetAbilities(NewAbilities);
}

#undef REGISTER_INT
#undef REGISTER_FLOAT