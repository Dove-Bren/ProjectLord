// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/GameTeam.h"

#include "GameTeamState.generated.h"

class ACastle;
class ABuilding;
class AUnit;
class ARewardFlag;
class UUnitType;
class UBuildingType;
class ALordPlayerController;
class AMonsterTeamController;
class UVMGameTeamState;
class UPlayerSpell;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTeamGoldChanged, int);
DECLARE_MULTICAST_DELEGATE(FOnTeamUnitsChanged);
DECLARE_MULTICAST_DELEGATE(FOnTeamBuildingsChanged);
DECLARE_MULTICAST_DELEGATE(FOnTeamFlagsChanged);
DECLARE_MULTICAST_DELEGATE(FOnTeamSpellsChanged);

UCLASS(BlueprintType)
class PROJECTLORD_API AGameTeamState : public AActor
{
    GENERATED_BODY()

public:

    AGameTeamState();

    static AGameTeamState* Make(UObject* WorldContext, EGameTeam Team, TOptional<FVector> CenterLocation = NullOpt)
    {
        AGameTeamState* State = WorldContext->GetWorld()->SpawnActor<AGameTeamState>();
        State->SetTeam(Team);
        if (CenterLocation) State->SetTeamCenterLocation(CenterLocation.GetValue());
        return State;
    }

    FOnTeamGoldChanged OnTeamGoldChanged;
    FOnTeamUnitsChanged OnTeamUnitsChanged;
    FOnTeamFlagsChanged OnTeamFlagsChanged;
    FOnTeamBuildingsChanged OnTeamBuildingsChanged;
    FOnTeamSpellsChanged OnTeamSpellsChanged;

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PostInitProperties() override;
    virtual void PostNetInit() override;

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
    TOptional<FVector> GetTeamCenterLocation() const { return TeamCenterLocation; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetTeamCenterLocation(FVector Center);

    UFUNCTION(BlueprintPure, Category = "Team")
    ALordPlayerController* GetPrimaryPlayerController() const { return PrimaryController.IsValid() ? PrimaryController.Get() : nullptr; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetPrimaryPlayerController(ALordPlayerController* Controller);

    UFUNCTION(BlueprintPure, Category = "Team")
    AMonsterTeamController* GetPrimaryMonsterController() const { return MonsterController.IsValid() ? MonsterController.Get() : nullptr; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetPrimaryMonsterController(AMonsterTeamController* Controller);

    // Warning: Units may not have replicated yet when calling this,
    // so make sure to IsValid check each item before using!
    UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<AUnit*>& GetUnits() const { return TeamUnits; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void AddUnit(AUnit* Unit);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void RemoveUnit(AUnit* Unit);

    // Get all buildings associated with this team.
    // Warning: Buildings may not have replicated yet when calling this,
    // so make sure to IsValid check each item before using!
    UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<ABuilding*>& GetBuildings() const { return TeamBuildings; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void AddBuilding(ABuilding* Building);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void RemoveBuilding(ABuilding* Building);

    // Warning: Flags may not have replicated yet when calling this,
    // so make sure to IsValid check each item before using!
    UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<ARewardFlag*>& GetFlags() const { return TeamFlags; }

    UFUNCTION(BlueprintCallable, Category = "Team")
    void AddFlag(ARewardFlag* Flag);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void RemoveFlag(ARewardFlag* Flag);

    UFUNCTION(BlueprintCallable, Category = "Team")
    void AddDeadHero(/*TODO: Saved, trimmed hero info?*/);

    UFUNCTION(BlueprintPure, Category = "Team")
    int GetDeadHeroCount() const { return DeadHeroCount; }

    // TODO
    /*UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<FHeroData>& GetDeadHeroes() const { return DeadHeroes; }*/

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

    // Spells we have based on town
    UFUNCTION(BlueprintPure, Category = "Team")
    const TArray<UPlayerSpell*>& GetCurrentSpells() const { return CurrentSpells; }

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_Team, Category = "Team")
    EGameTeam Team;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Gold, Category = "Team")
    int Gold;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_DeadHeroCount, Category = "Team")
    int DeadHeroCount;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_TeamCastle, Category = "Team")
    TWeakObjectPtr<ACastle> TeamCastle;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Team")
    TWeakObjectPtr<ALordPlayerController> PrimaryController;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Team")
    TWeakObjectPtr<AMonsterTeamController> MonsterController;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_TeamUnits, Category = "Team")
    TArray<AUnit*> TeamUnits;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_TeamBuildings, Category = "Team")
    TArray<ABuilding*> TeamBuildings;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_TeamFlags, Category = "Team")
    TArray<ARewardFlag*> TeamFlags;

    TMap<ABuilding*, TArray<AUnit*>> RepairWorkers;
    TMap<ABuilding*, TArray<AUnit*>> TaxWorkers;

    TArray<UPlayerSpell*> CurrentSpells;

    // Mostly useful for spacing players out. Optionally signifies the locational 'center'
    // of a team, which prevents new players from spawning too close.
    TOptional<FVector> TeamCenterLocation;

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<UVMGameTeamState> ViewModel;

    UFUNCTION()
    void OnRep_Team();
    UFUNCTION()
    void OnRep_Gold();
    UFUNCTION()
    void OnRep_DeadHeroCount();
    UFUNCTION()
    void OnRep_TeamCastle();
    UFUNCTION()
    void OnRep_TeamUnits();
    UFUNCTION()
    void OnRep_TeamBuildings();
    UFUNCTION()
    void OnRep_TeamFlags();

    UFUNCTION()
    void OnUnitFinalDeath(AUnit* Unit);

    void RefreshSpells();
    TSet<UPlayerSpell*> DiscoverSpellsFromBuildings() const; // uncached
};