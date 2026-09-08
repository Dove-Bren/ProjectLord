// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once


namespace FAttributeListener
{
	DECLARE_DELEGATE_OneParam(FSetIntValue, int);
	DECLARE_DELEGATE_OneParam(FSetFloatValue, float);

	template<typename T,
		typename = typename std::enable_if<std::is_convertible<float, T>::value>::type>
	void RegisterAndCall(UObject* Self, UAbilitySystemComponent* ASC, FGameplayAttribute Attribute, TDelegate<void(T InValue)> Setter)
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

#define _MAKE_ATTRIB_GET(AttributeName) Get##AttributeName##Attribute()
#define _MAKE_ATTRIB_CALLBACK(AttributeName) &ThisClass::Set##AttributeName


// Register a callback named "ThisClass::Set{AttributeName}" to get called whenever {AttributeName} is changed, and then also call it once immediately.
// Requires "AttributeSet" and "ASC" to be defined locally, like
//    auto ASC = Component->GetAbilitySubsystemComponent();
//    auto AttributeSet = Component->GetCombatAttributeSet();
#define REGISTER_ATTR_LISTENER_INT(AttributeName)\
	REGISTER_ATTR_LISTENER(AttributeName, this, FAttributeListener::FSetIntValue::CreateUObject(this, _MAKE_ATTRIB_CALLBACK(AttributeName)))

// Register a callback named "ThisClass::Set{AttributeName}" to get called whenever {AttributeName} is changed, and then also call it once immediately.
// Requires "AttributeSet" and "ASC" to be defined locally, like
//    auto ASC = Component->GetAbilitySubsystemComponent();
//    auto AttributeSet = Component->GetCombatAttributeSet();
#define REGISTER_ATTR_LISTENER_FLOAT(AttributeName) \
	REGISTER_ATTR_LISTENER(AttributeName, this, FAttributeListener::FSetFloatValue::CreateUObject(this, _MAKE_ATTRIB_CALLBACK(AttributeName)))


// Like REGISTER_INT but you provide the object ptr and FAttributeListener::FSetXValue yourself.
// Requires "AttributeSet" and "ASC" to be defined locally, like
//    auto ASC = Component->GetAbilitySubsystemComponent();
//    auto AttributeSet = Component->GetCombatAttributeSet();
#define REGISTER_ATTR_LISTENER(AttributeName, Self, Listener)\
	FAttributeListener::RegisterAndCall(Self, ASC, AttributeSet->_MAKE_ATTRIB_GET(AttributeName), Listener)
