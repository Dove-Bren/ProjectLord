// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/Ability/CombatAbility.h"

#include "AbilitySystemComponent.h"

#include "LordLogging.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Units/Creature.h"
#include "Gameplay/Units/Unit.h"
#include "UI/ViewModels/CombatAbilityViewModel.h"

UCombatAbility::UCombatAbility()
{
	// We use view models that are generated at runtime, so we can't 
	//InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// TODO: Figure out a way to make this work better. We _should_ be able to
	// use non-instanced, and create the view model on creation.
	// PostInitProperties didn't seem to work when I tried using it before, but I
	// don't remember why.
}

UVMCombatAbility* UCombatAbility::GetOrCreateViewModel()
{
	// Note: CANNOT call something like this on non-instanced abilities.
	// The contract for non-instanced states only defaults can be changed (via BP suclasses)
	// and any other change is not supported.
	// And indeed, doing this changes the ViewModel pointer on _all_ CombatAbility instances even
	// of different types.
	// This doesn't even work when using the `InstancedPerActor' policy, and I'm not sure why.
	// For now, just always making a new one which is very wasteful.
	//if (!ViewModel)
	UVMCombatAbility* VM;
	{
		VM = CreateLordVM<UVMCombatAbility>(this);
		VM->SetAbilityName(GetAbilityName());
		VM->SetDescription(GetAbilityDescription());
		VM->SetIcon(GetAbilityIcon());
	}

	return VM;
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

bool UCombatAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	/*if (auto RequiredCategory = GetRequiredTargetCategory())
	{
		// GAS doesn't provide target here?
	}*/

	return true;
}

bool UCombatAbility::CheckTargetValid(const AActor* Target) const
{
	if (!CanTargetBuildings() && Cast<ABuilding>(Target))
	{
		return false;
	}

	if (auto RequiredCategory = GetRequiredTargetCategory())
	{
		// Must match category. Find true path, but fall back to false.
		if (const ACreature* Creature = Cast<ACreature>(Target))
		{
			if (Creature->GetCategory() != RequiredCategory.GetValue())
			{
				return false;
			}
		}
	}

	return true;
}

bool UCombatAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
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

UGameplayEffect* UCombatAbility::GetCostGameplayEffect() const
{
	UGameplayEffect* Cost = Super::GetCostGameplayEffect();
	// Encoding as a cost GE doesn't really buy us anything, so don't bother
	/*if (!Cost)
	{
		Cost = ManaCostEffectOverride;
	}*/

	return Cost;
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

	// Encoding as a cost GE doesn't really buy us anything, so don't bother
	//else if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, ManaCost))
	//{
	//	if (ManaCost > 0)
	//	{
	//		if (Super::GetCostGameplayEffect())
	//		{
	//			UE_LOG(LordCombatAbility, Error, TEXT("Trying to set a mana cost, but ability already has a cost effect. This will not work!"));
	//			// Fall thhrough and make it anyways, so that all someone has to do is delete the existing cost
	//		}

	//		ManaCostEffectOverride = MakeManaCostGE(this, ManaCost);
	//	}
	//}
}
#endif // WITH_EDITOR  

/*static*/ UGameplayEffect* UCombatAbility::MakeManaCostGE(UObject* Outer, int Cost)
{
	UGameplayEffect* GE_ManaCost = NewObject<UGameplayEffect>(Outer, TEXT("ManaCostMod"));
	GE_ManaCost->DurationPolicy = EGameplayEffectDurationType::Instant;

	// Units heal while visiting
	FGameplayModifierInfo Mod;
	Mod.Attribute = UCombatAttributeSet::GetManaAttribute();
	Mod.ModifierOp = EGameplayModOp::AddBase;
	Mod.ModifierMagnitude = FScalableFloat(Cost);
	GE_ManaCost->Modifiers.Add(Mod);

	return GE_ManaCost;
}

