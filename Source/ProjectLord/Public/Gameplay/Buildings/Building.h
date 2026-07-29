// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/BuildingTypes.h"
#include "Gameplay/Combat/CombatTypes.h"

#include "Building.generated.h"

class ACreature;
class UBuildingType;
class UCombatAttributeSet;
class UAbilitySystemComponent;
class UCombatComponent;
class ABuildingController;
class USelectionComponent;

UCLASS(Blueprintable)
class PROJECTLORD_API ABuilding : public APawn
{
    GENERATED_BODY()

public:
    ABuilding();

    UBuildingType* GetBuildingType() const { return BuildingType; }
    const TArray<ACreature*> GetBuildingResidents() const { return Residents; }
    const TArray<ACreature*> GetBuildingVisitors() const { return Visitors; }

    UFUNCTION(BlueprintPure)
    ABuildingController* GetBuildingController() const;

    UFUNCTION(BlueprintPure)
    EUnitTeam GetTeam() const { return Team; }

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    bool RemoveResident(const ACreature* Resident);

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    void RemoveAllResidents();

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    bool AddResident(ACreature* Resident);

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    void RemoveAllVisitors();

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    void AddVisitor(ACreature* Visitor);

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    void RemoveVisitor(const ACreature* Visitor);

    UFUNCTION(BlueprintCallable)
    void SetBuildingGold(int InGold) { BuildingGold = InGold; }



    UFUNCTION(BlueprintPure)
    FVector GetBuildingEntrance() const;

    UFUNCTION(BlueprintPure)
    UCombatAttributeSet* GetCombatAttributeSet() const { return CombatAttributeSet; }

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    TObjectPtr<UBuildingType> BuildingType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    FText BuildingCustomName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bSupportsLevel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bSupportsResidents;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bSupportsVisitors;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bSupportsGoods;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bSupportsGold;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    int GoldGeneratedPerDay;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    FVector BuildingEntranceOffset;

    UPROPERTY(EditDefaultsOnly, Category = "Building|Definition", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.UnitBaseAttributes"))
    TObjectPtr<UDataTable> BuildingAttributeValues;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
    EUnitTeam Team;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building|Attributes")
    TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Selection")
    TObjectPtr<USelectionComponent> SelectionComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Building|Contents")
    int BuildingGold;

    // Creatures that consider this building their home
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Contents")
    TArray<ACreature*> Residents;

    // Creatures that are actively inside this building instead of out on the map
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Contents")
    TArray<ACreature*> Visitors;

    // TODO: Goods/services

    // TODO: Actions (like queue a hero recruit, start building upgrade, research something...)

    //UFUNCTION(BlueprintCallable)
    //ACreature* SpawnCreature(UCreatureType);

    virtual void SetupBaseAttributes();

    UFUNCTION()
    void HandleDeath();

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
};
