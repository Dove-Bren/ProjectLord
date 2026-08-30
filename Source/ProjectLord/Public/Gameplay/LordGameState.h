// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameStateBase.h"
#include "Gameplay/GameTeam.h"

#include "LordGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameDayChange, int, GameDays);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameTimeChange, double, GameDays);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSpeedChange, float, GameSpeed);

UCLASS(Blueprintable)
class PROJECTLORD_API ALordGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ALordGameState();

    UPROPERTY(BlueprintAssignable, Category = "GameTime")
    FOnGameDayChange OnGameDayChange;

    UPROPERTY(BlueprintAssignable, Category = "GameTime")
    FOnGameTimeChange OnGameTimeChange;

    UPROPERTY(BlueprintAssignable, Category = "GameTime")
    FOnGameSpeedChange OnGameSpeedChange;

    UFUNCTION(BlueprintPure, Category = "GameTime")
    double GetGameDays() const { return GameDays; }

    UFUNCTION(BlueprintPure, Category = "GameTime")
    int GetWholeGameDays() const { return FMath::FloorToInt(GetGameDays()); }

    UFUNCTION(BlueprintPure, Category = "GameTime")
    float GetGameSpeed() const { return GameSpeed; }

    UFUNCTION(BlueprintCallable, Category = "GameTime")
    void SetGameSpeed(float InSpeed);

    UFUNCTION(BlueprintCallable, Category = "GameTime")
    void SetGamePaused(bool bPaused);

    UFUNCTION(BlueprintPure, Category = "GameTeam")
    AGameTeamState* GetTeam(EGameTeam Team) { return GameTeams[Team]; }

    UFUNCTION(BlueprintPure, Category = "UI")
    class UVMLordGameState* GetViewModel() const { return ViewModel; }

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_GameDays, Category = "GameTime")
    double GameDays;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_GameSpeed, Category = "GameTime")
    float GameSpeed;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_GameTeams, Category = "GameTeam")
    TArray<AGameTeamState*> GameTeamsArray; // Replicated ownership version

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "GameTeam")
    TMap<EGameTeam, AGameTeamState*> GameTeams;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TObjectPtr<class UVMLordGameState> ViewModel;

    UFUNCTION()
    void OnRep_GameDays(double PrevGameDays);

    UFUNCTION()
    void OnRep_GameSpeed(float PrevGameSpeed);

    UFUNCTION()
    void OnRep_GameTeams(TArray<AGameTeamState*> PrevTeams);

    void SetGlobalTimeDilation(float Rate);

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
