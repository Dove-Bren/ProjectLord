// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordAbilitySystemComponent.h"

#include "Net/UnrealNetwork.h"

ULordAbilitySystemComponent::ULordAbilitySystemComponent()
	: UAbilitySystemComponent()
{
	SetIsReplicatedByDefault(true);
}

void ULordAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Override replication mode on ActivatableAbilities to always replicate!
	RESET_REPLIFETIME(ThisClass, ActivatableAbilities);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ActivatableAbilities, Params);
}

void ULordAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
}

void ULordAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	OnAbilitiesChanged.Broadcast();

	// Mark the ability list dirty so it replicates
	//ActivatableAbilities.MarkArrayDirty();
}

void ULordAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);
	OnAbilitiesChanged.Broadcast();
}
