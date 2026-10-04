// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

template<typename FutureType>
class TMulticastFuture
{
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnFutureComplete, FutureType);
	using FDelegateType = typename FOnFutureComplete::FDelegate;

    // 1. Raw Lambda / Static Function binding
    template <typename FuncType>
    void OnComplete(FuncType&& Callback)
    {
        OnCompleteDelegate(FDelegateType::CreateLambda(Forward<FuncType>(Callback)));
    }

    // 2. Weak Lambda binding (Safest for UObjects!)
    template <typename UserObjectType, typename FuncType>
    void OnCompleteWeak(UserObjectType* UserObject, FuncType&& Callback)
    {
        OnCompleteDelegate(FDelegateType::CreateWeakLambda(UserObject, Forward<FuncType>(Callback)));
    }

    // 3. UObject Method binding (Mirrors .BindUObject)
    template <typename UserObjectType, typename MethodPtrType>
    void OnCompleteUObject(UserObjectType* UserObject, MethodPtrType InMethod)
    {
        OnCompleteDelegate(FDelegateType::CreateUObject(UserObject, InMethod));
    }

    // 4. SP (Shared Pointer) Method binding
    template <typename UserObjectType, typename MethodPtrType>
    void OnCompleteSP(TSharedRef<UserObjectType> UserObject, MethodPtrType InMethod)
    {
        OnCompleteDelegate(FDelegateType::CreateSP(UserObject, InMethod));
    }

	void OnCompleteDelegate(FDelegateType Callback)
	{
		if (FutureValue.IsSet())
		{
			Callback.ExecuteIfBound(FutureValue.GetValue());
		}
		else
		{
			Event.Add(MoveTemp(Callback));
		}
	}

	void Complete(FutureType Value)
	{
		check(!FutureValue.IsSet());
		FutureValue = Value;
		Event.Broadcast(Value);
	}

protected:

	FOnFutureComplete Event;
	TOptional<FutureType> FutureValue = NullOpt;
};