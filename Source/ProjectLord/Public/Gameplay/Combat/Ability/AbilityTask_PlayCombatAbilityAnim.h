// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "Gameplay/Combat/Ability/AbilityEnums.h"

#include "AbilityTask_PlayCombatAbilityAnim.generated.h"

UCLASS()
class PROJECTLORD_API UAbilityTask_PlayCombatAbilityAnim : public UAbilityTask_PlayMontageAndWait
{
    GENERATED_BODY()

public:

	// Play the combat animation defined in the ability definition on the ability user.
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "PlayCombatAnimationAndWait",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_PlayCombatAbilityAnim* CreatePlayCombatAnimationAndWaitProxy(UGameplayAbility* OwningAbility,
		FName TaskInstanceName, float InRate = 1.f, FName InStartSection = NAME_None, bool bInStopWhenAbilityEnds = true, float InAnimRootMotionTranslationScale = 1.f, float InStartTimeSeconds = 0.f, bool bInAllowInterruptAfterBlendOut = false);
    
	// Play a specific combat animation on the ability user
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "PlaySpecificCombatAnimationAndWait",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_PlayCombatAbilityAnim* CreatePlaySpecificCombatAnimationAndWaitProxy(UGameplayAbility* OwningAbility,
		FName TaskInstanceName, EAbilityAnimType Animation, float InRate = 1.f, FName InStartSection = NAME_None, bool bInStopWhenAbilityEnds = true, float InAnimRootMotionTranslationScale = 1.f, float InStartTimeSeconds = 0.f, bool bInAllowInterruptAfterBlendOut = false);
};
