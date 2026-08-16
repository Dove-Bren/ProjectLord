// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Generic/CombatDataViewModel.h"

#include "AbilitySystemComponent.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Combat/GameplayEffect/VisibleGameplayEffect.h"
#include "UI/ViewModels/GameplayEffectVM.h"

namespace 
{
namespace AttributeWrapper
{
	DECLARE_DELEGATE_OneParam(FSetIntValue, int);
	DECLARE_DELEGATE_OneParam(FSetFloatValue, float);

	template<typename T,
		typename = typename std::enable_if<std::is_convertible<float, T>::value>::type>
	void RegisterAndCall(UVMCombatData* Self, UAbilitySystemComponent* ASC, FGameplayAttribute Attribute, TDelegate<void(T InValue)> Setter)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Attribute)
			.AddWeakLambda(Self, [Setter](const FOnAttributeChangeData& ChangeData)
				{
					Setter.ExecuteIfBound(static_cast<T>(ChangeData.NewValue));
				});
		bool bIgnored;
		Setter.ExecuteIfBound(static_cast<T>(ASC->GetGameplayAttributeValue(Attribute, bIgnored)));
	}
}
}

#define MAKE_ATTRIB_GET(AttributeName) Get##AttributeName##Attribute()

#define REGISTER_INT(AttributeName)\
	AttributeWrapper::RegisterAndCall(this, ASC, AttributeSet->MAKE_ATTRIB_GET(AttributeName), AttributeWrapper::FSetIntValue::CreateUObject(this, &UVMCombatData::Set##AttributeName));

#define REGISTER_FLOAT(AttributeName) \
	AttributeWrapper::RegisterAndCall(this, ASC, AttributeSet->MAKE_ATTRIB_GET(AttributeName), AttributeWrapper::FSetFloatValue::CreateUObject(this, &UVMCombatData::Set##AttributeName));

void UVMCombatData::Init(UCombatComponent* Component)
{
	// Register listeners and initial values
	auto ASC = Component->GetAbilitySubsystemComponent();
	auto AttributeSet = Component->GetCombatAttributeSet();

	REGISTER_INT(Health);
	REGISTER_INT(MaxHealth);

	REGISTER_INT(Mana);
	REGISTER_INT(MaxMana);

	REGISTER_INT(MeleeDefense);
	REGISTER_INT(RangedDefense);
	REGISTER_INT(MagicDefense);

	REGISTER_INT(MeleeDamage);
	REGISTER_INT(RangedDamage);
	REGISTER_INT(MagicDamage);

	REGISTER_FLOAT(Sight);
	REGISTER_FLOAT(AttackRange);

	Component->OnTargetChange.AddDynamic(this, &UVMCombatData::OnTargetChange);
	SetTarget(Component->GetCombatTarget());

	Component->OnInvulnerabilityChange.AddDynamic(this, &UVMCombatData::OnInvulnerabilityChange);
	SetInvulnerable(Component->IsInvulnerable());

	Component->OnEffectsChange.AddDynamic(this, &UVMCombatData::OnEffectsChange);
	SetupEffects(Component->GetActiveVisibleEffects());
}

void UVMCombatData::OnEffectsChange(UCombatComponent* SelfComponent)
{
	SetupEffects(SelfComponent->GetActiveVisibleEffects());
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

#undef REGISTER_INT
#undef REGISTER_FLOAT