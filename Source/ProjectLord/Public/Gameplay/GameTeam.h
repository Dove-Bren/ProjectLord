// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameTeam.generated.h"

class ABuilding;
class UVMGameTeamState;

UENUM(BlueprintType)
enum class EGameTeam : uint8
{
    Monster,
    Player1,
    Player2,
    Neutral,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTeamGoldChanged, int);

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

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure, Category = "Team")
    EGameTeam GetTeam() const { return Team; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetTeam(EGameTeam InTeam) { Team = InTeam; }

    UFUNCTION(BlueprintPure, Category = "Team")
    int GetGold() const { return Gold; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    int AddGold(int InChange);

    UFUNCTION(BlueprintPure, Category = "Team")
    ABuilding* GetCastle() const { return TeamCastle.IsValid() ? TeamCastle.Get() : nullptr; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetCastle(ABuilding* Castle);

    UFUNCTION(BlueprintPure, Category = "Team")
    UVMGameTeamState* GetViewModel() const { return ViewModel; }

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Team")
    EGameTeam Team;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
    int Gold;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Team")
    TWeakObjectPtr<ABuilding> TeamCastle;

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<UVMGameTeamState> ViewModel;
};