// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameTeam.generated.h"

class ABuilding;
class AUnit;
class ARewardFlag;
class UVMGameTeamState;

UENUM(BlueprintType)
enum class EGameTeam : uint8
{
    Monster,
    Player1,
    Player2,
    Neutral,

    MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EGameTeam, EGameTeam::MAX);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTeamGoldChanged, int);
DECLARE_MULTICAST_DELEGATE(FOnTeamUnitsChanged);
DECLARE_MULTICAST_DELEGATE(FOnTeamFlagsChanged);

UCLASS(BlueprintType)
class PROJECTLORD_API AGameTeamState : public AActor
{
    GENERATED_BODY()

public:

    AGameTeamState();

    static AGameTeamState* Make(UObject* WorldContext, EGameTeam Team)
    {
        AGameTeamState* State = WorldContext->GetWorld()->SpawnActor<AGameTeamState>();
        State->SetTeam(Team);
        return State;
    }

    FOnTeamGoldChanged OnTeamGoldChanged;
    FOnTeamUnitsChanged OnTeamUnitsChanged;
    FOnTeamFlagsChanged OnTeamFlagsChanged;

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure, Category = "Team")
    EGameTeam GetTeam() const { return Team; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetTeam(EGameTeam InTeam);

    UFUNCTION(BlueprintPure, Category = "Team")
    int GetGold() const { return Gold; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    int AddGold(int InChange);

    UFUNCTION(BlueprintPure, Category = "Team")
    ABuilding* GetCastle() const { return TeamCastle.IsValid() ? TeamCastle.Get() : nullptr; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetCastle(ABuilding* Castle);

    UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<AUnit*>& GetUnits() const { return TeamUnits; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void AddUnit(AUnit* Unit);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void RemoveUnit(AUnit* Unit);

    UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<ARewardFlag*>& GetFlags() const { return TeamFlags; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void AddFlag(ARewardFlag* Flag);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void RemoveFlag(ARewardFlag* Flag);

    UFUNCTION(BlueprintPure, Category = "Team")
    UVMGameTeamState* GetViewModel() const { return ViewModel; }

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "Team")
    EGameTeam Team;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Team")
    int Gold;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Team")
    TWeakObjectPtr<ABuilding> TeamCastle;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Team")
    TArray<AUnit*> TeamUnits;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Team")
    TArray<ARewardFlag*> TeamFlags;

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<UVMGameTeamState> ViewModel;

    UFUNCTION()
    void OnUnitFinalDeath(AUnit* Unit);
};