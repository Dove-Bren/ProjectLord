// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

template<typename FutureType>
class TMulticastFuture
{
private:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnFutureCompleteParam, FutureType);
    DECLARE_MULTICAST_DELEGATE(FOnFutureCompleteVoid);

public:

	
	using FDelegateType = std::conditional_t<std::is_void_v<FutureType>,
        typename FOnFutureCompleteVoid::FDelegate,
        typename FOnFutureCompleteParam::FDelegate
    >;

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
            if constexpr (std::is_void_v<FutureType>)
            {
                Callback.ExecuteIfBound();
            }
            else
            {
                Callback.ExecuteIfBound(FutureValue.GetValue());
            }
		}
		else
		{
			Event.Add(MoveTemp(Callback));
		}
	}

	void Complete(std::conditional_t<std::is_void_v<FutureType>, bool, FutureType> Value)
	{
		check(!FutureValue.IsSet());
		FutureValue = Value;

        if constexpr (std::is_void_v<FutureType>)
        {
            Event.Broadcast();
        }
        else
        {
            Event.Broadcast(Value);
        }
	}

protected:

	std::conditional_t<std::is_void_v<FutureType>, FOnFutureCompleteVoid, FOnFutureCompleteParam> Event;
	TOptional<std::conditional_t<std::is_void_v<FutureType>, bool, FutureType>> FutureValue = NullOpt;
    // Note: bool future when doing void just so we have something although bool value means nothing
};