// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GameTeam.h"
#include "RewardFlag.generated.h"

class UVMRewardFlag;
class USelectionComponent;

UENUM(BlueprintType)
enum class ERewardFlagType : uint8
{
    Explore,
    Defend,
    Attack,
    Fear,
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRewardFlagRewardChanged, ARewardFlag*, int);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRewardFlagInterestChanged, ARewardFlag*, int);

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

    FOnRewardFlagRewardChanged OnRewardFlagRewardChanged;
    FOnRewardFlagInterestChanged OnRewardFlagInterestChanged;

    UFUNCTION(BlueprintPure, Category = "Reward Flag")
    ERewardFlagType GetFlagType() const { return FlagType; }
    UFUNCTION(BlueprintCallable, Category = "Reward Flag")
    void SetFlagType(ERewardFlagType InType);

    UFUNCTION(BlueprintPure, Category = "Reward Flag")
    EGameTeam GetTeam() const { return Team; }
    UFUNCTION(BlueprintCallable, Category = "Reward Flag")
    void SetTeam(EGameTeam InTeam);

    UFUNCTION(BlueprintPure, Category = "Reward Flag")
    int GetReward() const { return Reward; }
    UFUNCTION(BlueprintCallable, Category = "Reward Flag")
    void SetReward(int inReward);

    UFUNCTION(BlueprintPure, Category = "Reward Flag")
    bool IsAttached() const { return !!AttachedUnit; }
    UFUNCTION(BlueprintPure, Category = "Reward Flag")
    AUnit* GetAttachedUnit() const { return AttachedUnit; }
    UFUNCTION(BlueprintCallable, Category = "Reward Flag")
    void SetAttachedUnit(AUnit* InInit);

    UFUNCTION(BlueprintPure, Category = "Reward Flag")
    const TSet<AUnit*> GetInterestedUnits() const { return InterestedUnits; }
    UFUNCTION(BlueprintPure, Category = "Reward Flag")
    int GetNumInterestedUnits() const { return InterestedUnitCount; }
    UFUNCTION(BlueprintCallable, Category = "Reward Flag")
    void AddInterestedUnit(AUnit* Unit);
    UFUNCTION(BlueprintCallable, Category = "Reward Flag")
    void RemoveInterestedUnit(AUnit* Unit);


    UFUNCTION(BlueprintPure, Category = "Reward Flag")
    FVector GetGroundLocation() const;

    UVMRewardFlag* GetViewModel() const { return ViewModel; }


protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Reward Flag")
    ERewardFlagType FlagType;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_Team, Category = "Reward Flag")
    EGameTeam Team;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_Reward, Category = "Reward Flag")
    int Reward;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_AttachedUnit, Category = "Reward Flag")
    TObjectPtr<AUnit> AttachedUnit;

    // Only kept up-to-date on server
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Reward Flag")
    TSet<AUnit*> InterestedUnits;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_InterestedUnits, Category = "Reward Flag")
    int InterestedUnitCount;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Reward Flag")
    TObjectPtr<UVMRewardFlag> ViewModel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Selection")
    TObjectPtr<USelectionComponent> SelectionComponent;

    UFUNCTION()
    void OnRep_Team(EGameTeam OldValue);
    UFUNCTION()
    void OnRep_Reward(int OldValue);
    UFUNCTION()
    void OnRep_AttachedUnit(AUnit* OldValue);
    UFUNCTION()
    void OnRep_InterestedUnits(int OldValue);

    UFUNCTION()
    void OnUnitDeath(AUnit* Unit);

    UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Reward Flag")
    FText GetFlagName() const;

    UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Reward Flag")
    FText GetFlagDescription() const;
};
