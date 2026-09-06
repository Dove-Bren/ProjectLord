// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/RewardFlag.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "RewardFlagViewModel.generated.h"

class UVMCombatData;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMRewardFlag : public UVMLordBase
{
    GENERATED_BODY()

public:
    ERewardFlagType GetType() const { return Type; }
    void SetType(ERewardFlagType InType) { UE_MVVM_SET_PROPERTY_VALUE(Type, InType); }

    EGameTeam GetTeam() const { return Team; }
    void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

    int GetReward() const { return Reward; }
    void SetReward(int InReward) { UE_MVVM_SET_PROPERTY_VALUE(Reward, InReward); }

    UFUNCTION(FieldNotify, BlueprintPure)
    bool IsAttached() const { return !!AttachedComponentVM; }

    UVMCombatData* GetAttachedComponentVM() const { return AttachedComponentVM; }
    void SetAttachedCombatComponent(UVMCombatData* InVM) { UE_MVVM_SET_PROPERTY_VALUE(AttachedComponentVM, InVM); UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IsAttached); }

    // TODO: Will need actual unit list for UI
    int GetInterestedCount() const { return InterestedCount; }
    void SetInterestedCount(int InCount) { UE_MVVM_SET_PROPERTY_VALUE(InterestedCount, InCount); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Reward Flag")
    ERewardFlagType Type;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Reward Flag")
    EGameTeam Team;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Reward Flag")
    int Reward;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Reward Flag")
    int InterestedCount;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Reward Flag")
    TObjectPtr<UVMCombatData> AttachedComponentVM;
};
