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
    using FDelegateValType = std::conditional_t<std::is_void_v<FutureType>, bool, FutureType>;

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

	void Complete(FDelegateValType Value)
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

    bool IsComplete() const { return FutureValue.IsSet(); }

    template<typename T>
    static void All(TDelegate<void(TArray<T>)> OnComplete, const TArray<TMulticastFuture<T>>& Futures)
    {
        check(Futures.Num() > 1);
        TArray<T>* Values = new TArray<T>(); // not thread safe!
        Values->Reserve(Futures.Num());

        const int Expected = Futures.Num();
        auto Check = [Values, Expected, OnComplete = MoveTemp(OnComplete)](T Val) {
            Values->Add(Val);

            if (Values->Num() >= Expected)
            {
                OnComplete.ExecuteIfBound(*Values);
                delete Values;
            }
        };

        for (auto Future : Futures)
        {
            Future.OnComplete(Check);
        }
    }

    static void AllVoid(TDelegate<void()> OnComplete, const TArray<TMulticastFuture<void>*>& Futures)
    {
        check(Futures.Num() > 1);

        int* Count = new int(); // not thread safe!
        *Count = 0;
        const int Expected = Futures.Num();
        auto Check = [Count, Expected, OnComplete = MoveTemp(OnComplete)]() mutable {
            (*Count)++;

            if (*Count >= Expected)
            {
                OnComplete.ExecuteIfBound();
                delete Count;
            }
            };

        for (auto Future : Futures)
        {
            Future->OnComplete(Check);
        }
    }

protected:

	std::conditional_t<std::is_void_v<FutureType>, FOnFutureCompleteVoid, FOnFutureCompleteParam> Event;
	TOptional<FDelegateValType> FutureValue = NullOpt;
    // Note: bool future when doing void just so we have something although bool value means nothing
};