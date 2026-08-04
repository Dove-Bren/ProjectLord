// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/AbilityTask_PlayCombatAbilityAnim.h"

#include "AbilitySystemGlobals.h"

#include "Gameplay/Combat/Ability/CombatAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_PlayCombatAbilityAnim)

/*static*/ UAbilityTask_PlayCombatAbilityAnim* UAbilityTask_PlayCombatAbilityAnim::CreatePlayCombatAnimationAndWaitProxy(
	UGameplayAbility* OwningAbility, FName TaskInstanceName,
	float InRate, FName InStartSection, bool bInStopWhenAbilityEnds, float InAnimRootMotionTranslationScale, float InStartTimeSeconds, bool bInAllowInterruptAfterBlendOut)
{
	UCombatAbility* CombatAbility = Cast<UCombatAbility>(OwningAbility);
	if (!ensure(CombatAbility))
	{
		return nullptr;
	}

	return UAbilityTask_PlayCombatAbilityAnim::CreatePlaySpecificCombatAnimationAndWaitProxy(OwningAbility, TaskInstanceName, CombatAbility->GetAbilityAnimation(),
		InRate, InStartSection, bInStopWhenAbilityEnds, InAnimRootMotionTranslationScale, InStartTimeSeconds, bInAllowInterruptAfterBlendOut
		);
}

/*static*/ UAbilityTask_PlayCombatAbilityAnim* UAbilityTask_PlayCombatAbilityAnim::CreatePlaySpecificCombatAnimationAndWaitProxy(
	UGameplayAbility* OwningAbility, FName TaskInstanceName, EAbilityAnimType Animation,
	float InRate, FName InStartSection, bool bInStopWhenAbilityEnds, float InAnimRootMotionTranslationScale, float InStartTimeSeconds, bool bInAllowInterruptAfterBlendOut)
{
	UCombatAbility* CombatAbility = Cast<UCombatAbility>(OwningAbility);
	if (!ensure(CombatAbility))
	{
		return nullptr;
	}

	UAnimMontage* MontageToPlay = CombatAbility->GetAbilityAnimationFromOwner(Animation);


	// If this was just a BlueprintFuncLibrary func, I wouldn't need to dupe this.
	// Copied from "UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy"
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(InRate);

	UAbilityTask_PlayCombatAbilityAnim* MyObj = NewAbilityTask<UAbilityTask_PlayCombatAbilityAnim>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->Rate = InRate;
	MyObj->StartSection = InStartSection;
	MyObj->AnimRootMotionTranslationScale = InAnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bInStopWhenAbilityEnds;
	MyObj->bAllowInterruptAfterBlendOut = bInAllowInterruptAfterBlendOut;
	MyObj->StartTimeSeconds = InStartTimeSeconds;

	return MyObj;

}
