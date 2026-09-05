// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"

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
class UBuildingConstructionFadeComponent;
class UNavModifierComponent;
class UBoxComponent;
class UFogOfWarComponent;
class UMinimapComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuildingLevelChanged, int);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuildingAvailableLevelChanged, int);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBuildingNeedsRepairsChanged, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBuildingHealthChanged, int /*Health*/, int /*MaxHealth*/);

UCLASS(Blueprintable)
class PROJECTLORD_API ABuilding : public APawn, public IGameplayTagAssetInterface
{
    GENERATED_BODY()

public:
    ABuilding();

    UBuildingType* GetBuildingType() const { return BuildingType; }

    FOnBuildingLevelChanged OnBuildingLevelChanged;
    FOnBuildingAvailableLevelChanged OnBuildingAvailableLevelChanged;
    FOnBuildingNeedsRepairsChanged OnBuildingNeedsRepairsChanged;
    FOnBuildingHealthChanged OnBuildingHealthChanged;

    UFUNCTION(BlueprintPure)
    ABuildingController* GetBuildingController() const;

    UFUNCTION(BlueprintPure)
    FText GetBuildingName() const;

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const { return Team; }

    UFUNCTION(BlueprintCallable)
    void SetTeam(EGameTeam InTeam);

    UFUNCTION(BlueprintPure)
    int GetBuildingLevel() const { return BuildingLevel; }

    UFUNCTION(BlueprintPure)
    int GetBuildingAvailableLevel() const { return BuildingAvailableLevel; }

    UFUNCTION(BlueprintPure)
    int GetBuildingMaxLevel() const { return MaxLevel; }

    UFUNCTION(BlueprintPure)
    bool IsMaxLevel() const { return GetBuildingLevel() >= GetBuildingMaxLevel(); }

    UFUNCTION(BlueprintPure)
    bool CanLevelUp() const;

    UFUNCTION(BlueprintCallable)
    void StartLevelUp();

    UFUNCTION(BlueprintPure)
    int GetBuildingGold() const { return BuildingGold; }

    UFUNCTION(BlueprintCallable)
    void SetBuildingGold(int InGold);

    UFUNCTION(BlueprintPure)
    int GetBuildingHealth() const;
    UFUNCTION(BlueprintPure)
    int GetBuildingMaxHealth() const;

    UFUNCTION(BlueprintCallable)
    void PlaceExitingUnit(AUnit* Unit);



    UFUNCTION(BlueprintPure)
    FVector GetBuildingEntrance() const;

    UFUNCTION(BlueprintPure)
    UCombatAttributeSet* GetCombatAttributeSet() const { return CombatAttributeSet; }

    UFUNCTION(BlueprintPure)
    UStaticMesh* GetBuildingMesh() const;

    UFUNCTION(BlueprintPure)
    bool IsUnderConstruction() const { return GetBuildingLevel() < GetBuildingAvailableLevel(); }

    UFUNCTION(BlueprintPure)
    bool WantsRepair() const;

    UFUNCTION(BlueprintNativeEvent)
    void HandleBuildingPlacement();

    UFUNCTION(BlueprintCallable)
    virtual void RefreshMesh();

    // Called by a repairer every time they repair the building
    UFUNCTION(BlueprintCallable)
    virtual void NotifyRepairAction();

    UFUNCTION(BlueprintCallable)
    virtual void NotifyRepairComplete();

    UFUNCTION(BlueprintPure)
    UStaticMeshComponent* GetBuildingMeshComponent() const { return BuildingMesh; }

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building|Definition")
    UBoxComponent* Collision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building|Definition")
    UStaticMeshComponent* BuildingMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building|Definition")
    UNavModifierComponent* NavMeshMod;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building|Definition")
    UBuildingConstructionFadeComponent* FadeComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    TObjectPtr<UFogOfWarComponent> FogOfWarComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Map")
    TObjectPtr<UMinimapComponent> MinimapComponent;

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

    UPROPERTY(EditDefaultsOnly, Category = "Building|Definition", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.AttributeBaseValue"))
    TArray<UDataTable*> BuildingLevelAttributeValues;

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

    // Active, functioning level of the building.
    // Note: can be 0 when building hasn't been built yet.
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Building")
    int BuildingLevel;

    // Level that the building can get to, if repaired enough
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Building")
    int BuildingAvailableLevel;

    UPROPERTY()
    TObjectPtr<UVMGold> GoldVM;

    virtual void SetupBaseAttributes();
    virtual void SetupSelectionData(USelectionComponent* InSelectionComponent);
    virtual void HandleBuildingUpgraded();
    virtual void SetAvailableLevel(int InAvailableLevel);

    void SetLevel(int InLevel) { BuildingLevel = InLevel; OnBuildingLevelChanged.Broadcast(BuildingLevel); }

    UFUNCTION(BlueprintImplementableEvent)
    void OnRepairActionReceived();

    UFUNCTION(BlueprintImplementableEvent)
    void OnRepairComplete();

    UFUNCTION(BlueprintImplementableEvent)
    void OnUpgradeComplete();

    UFUNCTION()
    virtual void HandleDeath();

    UFUNCTION()
    virtual void HandleGameDayChanged(int GameDay);

    UFUNCTION()
    virtual void HandleHealthChanged(int Health, int MaxHealth);

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void Tick(float DeltaSeconds) override;

    // Tag Interface
    virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
    virtual bool HasMatchingGameplayTag(FGameplayTag Tag) const override;
    virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
    virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
    // End tag interface
};
