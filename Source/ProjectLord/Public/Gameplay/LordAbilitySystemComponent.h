// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "LordAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilitiesChanged);

UCLASS()
class PROJECTLORD_API ULordAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    ULordAbilitySystemComponent();

    UPROPERTY(BlueprintAssignable)
    FOnAbilitiesChanged OnAbilitiesChanged;

protected:

    //// Manual extra copy of ActivatableAbilities that we replicate
    //UPROPERTY(ReplicatedUsing = OnRep_AbilityList)
    //TArray<FGameplayAbilitySpec> AbilityList;


    virtual void OnRep_ActivateAbilities() override;
    virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
    virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
