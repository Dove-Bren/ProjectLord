// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GameTeam.h"
#include "RewardFlag.generated.h"

class UVMRewardFlag;

UENUM(BlueprintType)
enum class ERewardFlagType : uint8
{
    Explore,
    Defend,
    Attack,
    Fear,
};

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API ARewardFlag : public AActor
{
    GENERATED_BODY()

public:
    ARewardFlag();

    static ARewardFlag* Make(const UObject* WorldContext, TSubclassOf<ARewardFlag> Class, ERewardFlagType Type, FVector At)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        ARewardFlag* Flag = WorldContext->GetWorld()->SpawnActor<ARewardFlag>(Class, At, {}, Params);
        Flag->SetFlagType(Type);
        return Flag;
    }

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    ERewardFlagType GetFlagType() const { return FlagType; }
    void SetFlagType(ERewardFlagType InType);

    EGameTeam GetTeam() const { return Team; }
    void SetTeam(EGameTeam InTeam);

    int GetReward() const { return Reward; }
    void SetReward(int inReward);

    bool IsAttached() const { return !!AttachedUnit; }
    AUnit* GetAttachedUnit() const { return AttachedUnit; }
    void SetAttachedUnit(AUnit* InInit);

    UVMRewardFlag* GetViewModel() const { return ViewModel; }


protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Flags")
    ERewardFlagType FlagType;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Flags")
    EGameTeam Team;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Flags")
    int Reward;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Flags")
    TObjectPtr<AUnit> AttachedUnit;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Flags")
    TObjectPtr<UVMRewardFlag> ViewModel;

    void RegisterFlag();
    void UnregisterFlag();

    UFUNCTION()
    void OnUnitDeath(AUnit* Unit);
};
