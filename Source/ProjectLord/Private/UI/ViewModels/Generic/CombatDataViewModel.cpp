// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Generic/CombatDataViewModel.h"

#include "AbilitySystemComponent.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"

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

	REGISTER_INT(BonusMeleeDamage);
	REGISTER_INT(BonusRangedDamage);
	REGISTER_INT(BonusMagicDamage);

	REGISTER_FLOAT(Sight);
	REGISTER_FLOAT(AttackRange);

	Component->OnTargetChange.AddDynamic(this, &UVMCombatData::OnTargetChange);
	SetTarget(Component->GetCombatTarget());
}

#undef REGISTER_INT
#undef REGISTER_FLOAT