// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/PlayerSpell/PlayerSpellGameplayAbility.h"

#include "Gameplay/LordPlayerController.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpell.h"

UPlayerSpellGameplayAbility::UPlayerSpellGameplayAbility()
{
}

void UPlayerSpellGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Taken from Super() comments
	if (!ensure(TriggerEventData) || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{			
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	auto Player = Cast<ALordPlayerController>(TriggerEventData->Instigator);
	if (!ensure(Player))
	{
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	auto AtData = TriggerEventData->TargetData.Get(0);
	if (!ensure(AtData))
	{
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	auto At = AtData->GetEndPoint();

	auto Target = Cast<UCombatComponent>(TriggerEventData->OptionalObject);
	auto Spell = Cast<UPlayerSpell>(TriggerEventData->OptionalObject2);
	
	// What is the right way around these const casts? Passing it all through target data maybe?
	ActivatePlayerSpell(const_cast<ALordPlayerController*>(Player), At, const_cast<UCombatComponent*>(Target), const_cast<UPlayerSpell*>(Spell));
}

/*static*/ FGameplayEventData UPlayerSpellGameplayAbility::MakeEventPayload(ALordPlayerController* Player, FVector At, UCombatComponent* Target, UPlayerSpell* OriginalSpell)
{
	FGameplayEventData Payload;

	Payload.Instigator = Player;
	auto AtData = new FGameplayAbilityTargetData_SingleTargetHit(FHitResult({}, At));
	Payload.TargetData.Add(AtData);

	Payload.OptionalObject = Target;
	Payload.OptionalObject2 = OriginalSpell;

	return Payload;
}
