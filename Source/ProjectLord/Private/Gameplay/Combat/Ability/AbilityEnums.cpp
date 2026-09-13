// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/AbilityEnums.h"

#include "GameplayTags.h"
#include "Gameplay/LordGameplayTags.h"

/*static*/ FGameplayTag UAbilityEnumsFunctionLibrary::GetTagForTargetType(EAbilityTargetType Type)
{
	switch (Type)
	{
	case EAbilityTargetType::Enemy:
	default:
		return ULordGameplayTags::AbilityTargetEnemy();
	case EAbilityTargetType::Ally:
		return ULordGameplayTags::AbilityTargetAlly();
	case EAbilityTargetType::Self:
		return ULordGameplayTags::AbilityTargetSelf();
	}
}
