// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameTeam.generated.h"

class ACastle;
class ABuilding;
class AUnit;
class ARewardFlag;
class UUnitType;
class UBuildingType;
class ALordPlayerController;
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
DECLARE_MULTICAST_DELEGATE(FOnTeamBuildingsChanged);
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
    FOnTeamBuildingsChanged OnTeamBuildingsChanged;

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
    ACastle* GetCastle() const { return TeamCastle.IsValid() ? TeamCastle.Get() : nullptr; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetCastle(ACastle* Castle);

    UFUNCTION(BlueprintPure, Category = "Team")
    ALordPlayerController* GetPrimaryPlayerController() const { return PrimaryController.IsValid() ? PrimaryController.Get() : nullptr; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetPrimaryPlayerController(ALordPlayerController* Controller);

    UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<AUnit*>& GetUnits() const { return TeamUnits; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void AddUnit(AUnit* Unit);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void RemoveUnit(AUnit* Unit);

    UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<ABuilding*>& GetBuildings() const { return TeamBuildings; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void AddBuilding(ABuilding* Building);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void RemoveBuilding(ABuilding* Building);

    UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<ARewardFlag*>& GetFlags() const { return TeamFlags; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void AddFlag(ARewardFlag* Flag);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void RemoveFlag(ARewardFlag* Flag);

    UFUNCTION(BlueprintPure, Category = "Team")
    UVMGameTeamState* GetViewModel() const { return ViewModel; }


    // Team/Town convenience funcs
    UFUNCTION(BlueprintCallable, Category = "Team")
    TArray<AUnit*> GetTeamUnitsOfClass(TSubclassOf<AUnit> Type) const;

    UFUNCTION(BlueprintCallable, Category = "Team")
    TArray<AUnit*> GetTeamUnitsOfType(const UUnitType* Type) const;

    UFUNCTION(BlueprintCallable, Category = "Team")
    TArray<ABuilding*> GetTeamBuildingsOfClass(TSubclassOf<ABuilding> Type) const;

    UFUNCTION(BlueprintCallable, Category = "Team")
    TArray<ABuilding*> GetTeamBuildingsOfType(const UBuildingType* Type) const;


    // Team/Town build and repair worker assignments
    UFUNCTION(BlueprintCallable, Category = "Team")
    ABuilding* GetNextBuildingToRepair(AUnit* Worker);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void NotifyBuildingRepairComplete(ABuilding* Building);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void NotifyRepairWorkerAbandoned(AUnit* Worker, ABuilding* BuildingOptional);

    // ... Tax collecting assignments
    UFUNCTION(BlueprintCallable, Category = "Team")
    ABuilding* GetNextBuildingToTax(AUnit* Worker);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void NotifyBuildingTaxCollected(ABuilding* Building);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void NotifyTaxWorkerAbandoned(AUnit* Worker, ABuilding* BuildingOptional);

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "Team")
    EGameTeam Team;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Team")
    int Gold;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Team")
    TWeakObjectPtr<ACastle> TeamCastle;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Team")
    TWeakObjectPtr<ALordPlayerController> PrimaryController;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Team")
    TArray<AUnit*> TeamUnits;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Team")
    TArray<ABuilding*> TeamBuildings;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Team")
    TArray<ARewardFlag*> TeamFlags;

    TMap<ABuilding*, TArray<AUnit*>> RepairWorkers;
    TMap<ABuilding*, TArray<AUnit*>> TaxWorkers;

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<UVMGameTeamState> ViewModel;

    UFUNCTION()
    void OnUnitFinalDeath(AUnit* Unit);
};