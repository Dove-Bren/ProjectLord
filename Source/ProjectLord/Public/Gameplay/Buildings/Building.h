// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/BuildingTypes.h"
#include "Gameplay/GameTeam.h"

#include "Building.generated.h"

class UBuildingType;
class UCombatAttributeSet;
class UAbilitySystemComponent;
class UCombatComponent;
class ABuildingController;
class USelectionComponent;
class UStaticMesh;
class UStaticMeshComponent;
class UBuildingActionQueueComponent;
class UVMGold;
class UUnitType;

UCLASS(Blueprintable)
class PROJECTLORD_API ABuilding : public APawn
{
    GENERATED_BODY()

public:
    ABuilding();

    UBuildingType* GetBuildingType() const { return BuildingType; }

    UFUNCTION(BlueprintPure)
    ABuildingController* GetBuildingController() const;

    UFUNCTION(BlueprintPure)
    FText GetBuildingName() const;

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const { return Team; }

    UFUNCTION(BlueprintCallable)
    void SetBuildingGold(int InGold);

    UFUNCTION(BlueprintCallable)
    void PlaceExitingUnit(AUnit* Unit);



    UFUNCTION(BlueprintPure)
    FVector GetBuildingEntrance() const;

    UFUNCTION(BlueprintPure)
    UCombatAttributeSet* GetCombatAttributeSet() const { return CombatAttributeSet; }

    UFUNCTION(BlueprintPure)
    UStaticMesh* GetBuildingMesh() const;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building|Definition")
    UStaticMeshComponent* BuildingMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    TObjectPtr<UBuildingType> BuildingType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    FText BuildingCustomName;

    /*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bSupportsResidents;*/

    /*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bSupportsVisitors;*/

    /*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bSupportsGoods;*/

    /*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bSupportsGold;*/

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    int GoldGeneratedPerDay;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    int MaxLevel;

    // Meshes to match to each level of the building. There should be one entry in here
    // every 'MaxLevel' -- aka if MaxLevel is 3, there should be 3 entries.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    TArray<UStaticMesh*> BuildingLevelMeshes;

    // Meshes to cycle through when constructing the building. Mesh will be
    // selected automatically based on how far along construction is and how many
    // construction meshes are available.
    // For example, if there are 3 meshes, the first will be shown from 0-33%,
    // the second from 33% to 66%, and the third from 66% to 100%. At that point,
    // the first level mesh will be shown.
    // There should always be at least 1 of these.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    TArray<UStaticMesh*> BuildingConstructionMeshes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    FVector BuildingEntranceOffset;

    UPROPERTY(EditDefaultsOnly, Category = "Building|Definition", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.AttributeBaseValue"))
    TObjectPtr<UDataTable> BuildingAttributeValues;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Definition")
    bool bIndestructible = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
    EGameTeam Team;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building|Definition|Attributes")
    TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Selection")
    TObjectPtr<USelectionComponent> SelectionComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Building|Contents")
    int BuildingGold;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Building")
    int BuildingLevel;

    UPROPERTY()
    TObjectPtr<UVMGold> GoldVM;

    virtual void SetupBaseAttributes();
    virtual void SetupSelectionData(USelectionComponent* InSelectionComponent);

    int GetBuildingHealth() const;
    int GetBuildingMaxHealth() const;

    UFUNCTION()
    virtual void HandleDeath();

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void Tick(float DeltaSeconds) override;
};
