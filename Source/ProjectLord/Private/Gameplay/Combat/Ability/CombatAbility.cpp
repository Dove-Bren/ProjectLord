// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/CombatAbility.h"

#include "AbilitySystemComponent.h"

#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Units/Unit.h"
#include "UI/ViewModels/CombatAbilityViewModel.h"

UVMCombatAbility* UCombatAbility::GetOrCreateViewModel()
{
	if (!ViewModel)
	{
		ViewModel = CreateLordVM<UVMCombatAbility>(this);
		ViewModel->SetAbilityName(GetAbilityName());
		ViewModel->SetDescription(GetAbilityDescription());
		ViewModel->SetIcon(GetAbilityIcon());
	}

	return ViewModel;
}

UCombatComponent* UCombatAbility::GetOwnerComponent() const
{
	AActor* Owner = GetOwningActorFromActorInfo();
	return Owner->GetComponentByClass<UCombatComponent>();
}

UCombatComponent* UCombatAbility::GetOwnerTarget(bool bOnlyAlive) const
{
	auto Owner = GetOwnerComponent();
	if (!ensure(IsValid(Owner)))
	{
		return nullptr;
	}

	auto Target = Owner->GetCombatTarget();
	return IsValid(Target) && (!bOnlyAlive || Target->IsAlive())
		? Target
		: nullptr;
}

void UCombatAbility::ReportAbilityHit(UCombatComponent* HitComponent)
{
	if (!ensure(HitComponent))
	{
		return;
	}

	auto Owner = GetOwnerComponent();
	if (ensure(IsValid(Owner)))
	{
		Owner->NotifyOfAbilityHit(HitComponent);
	}
}

UAnimMontage* UCombatAbility::GetAbilityAnimationFromOwner(EAbilityAnimType Type) const
{
	AActor* Owner = GetOwningActorFromActorInfo();
	if (!ensure(Owner))
	{
		return nullptr;
	}

	AUnit* UnitOwner = Cast<AUnit>(Owner);
	if (!ensure(UnitOwner))
	{
		return nullptr;
	}

	return UnitOwner->GetAnimForAbilityType(Type);
}

bool UCombatAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// Also check mana, if applicable
	if (ManaCost > 0)
	{
		bool bFound;
		const int Mana = FMath::FloorToInt(ActorInfo->AbilitySystemComponent->GetGameplayAttributeValue(UCombatAttributeSet::GetManaAttribute(), bFound));
		if (!bFound || Mana < ManaCost)
		{
			return false;
		}
	}

	return true;
}

void UCombatAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	if (ManaCost > 0)
	{
		ActorInfo->AbilitySystemComponent->ApplyModToAttribute(UCombatAttributeSet::GetManaAttribute(), EGameplayModOp::AddBase, -ManaCost);
	}
}

#if WITH_EDITOR
void UCombatAbility::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, TargetType))
	{
		// Change tags to match.
		// Choosing to leave any previous one in there since the user can just uncheck that one manually or leave it if it makes sense.
		AbilityTags.AddTag(UAbilityEnumsFunctionLibrary::GetTagForTargetType(TargetType));
	}
}
#endif // WITH_EDITOR  

