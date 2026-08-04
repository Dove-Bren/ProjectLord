// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GameplayEffect.h"

#include "Gameplay/GameTeam.h"
#include "Gameplay/Units/UnitTypes.h"
#include "Gameplay/Combat/Ability/CombatAbility.h" // Including instead of forward declared to make dropdowns populate right

#include "Unit.generated.h"

class AUnitController;
class UAbilitySystemComponent;
class UCombatComponent;
class ULordUnitAttributeSet;
class UCombatAttributeSet;
class UVMUnit;
class UWidgetComponent;
class USelectionComponent;
class UUnitType;

struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;

UCLASS(Blueprintable)
class PROJECTLORD_API AUnit : public ACharacter
{
    GENERATED_BODY()

public:
    AUnit();


    UFUNCTION(BlueprintPure)
    AUnitController* GetUnitController() const;

    UFUNCTION(BlueprintPure, Category = "Unit")
    UUnitType* GetUnitType() const { return UnitType; }

    UFUNCTION(BlueprintPure, Category = "Unit")
    virtual FText GetUnitName() const { return (ensure(GetUnitType())) ? GetUnitType()->UnitName : FText::GetEmpty(); }

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const { return Team; }

    UFUNCTION(BlueprintCallable)
    void SetTeam(EGameTeam InTeam) { Team = InTeam; }

    UFUNCTION(BlueprintPure)
    bool IsDead() const;

    UFUNCTION(BlueprintPure)
    bool IsAlive() const { return !IsDead(); }

    UFUNCTION(BlueprintCallable, Category = "Actor")
    void FaceActor(AActor* OtherActor);

    UFUNCTION(BlueprintPure)
    UCombatAttributeSet* GetCombatAttributeSet() const { return CombatAttributeSet; }

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;

protected:

    // REMOVE
    UFUNCTION(BlueprintNativeEvent)
    void OnDeath();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Definision")
    TObjectPtr<UUnitType> UnitType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
    EGameTeam Team;

    UPROPERTY(EditDefaultsOnly, Category = "Attributes", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.AttributeBaseValue"))
    TObjectPtr<UDataTable> ClassAttributeDefaults;

    // Attributes to add for every level the unit gains.
    // Typically, just set to 1 (or so) damage of the type relevant to the class.
    UPROPERTY(EditDefaultsOnly, Category = "Attributes", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.AttributeBaseValue"))
    TObjectPtr<UDataTable> ClassAttributeGrowth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Selection")
    TObjectPtr<USelectionComponent> SelectionComponent;

    UPROPERTY()
    FActiveGameplayEffectHandle LevelDamageModHandle;

    virtual void RegisterAttributes();
    virtual void SetupBaseAttributes();
    virtual void SetupSelectionData(USelectionComponent* SelectionComponent);

    void ApplyLevelDamageMod(int Level);

private:
    void InitUnitVM();
    void AddHealthbarWidget();

    UPROPERTY()
    TObjectPtr<UWidgetComponent> HealthbarWidgetComponent;

    UPROPERTY()
    TObjectPtr<UVMUnit> UnitVM;

    UFUNCTION()
    void HandleDeath();

    UFUNCTION()
    void HandleAttack(AActor* Target, UCombatComponent* TargetComponent);

public:
    UFUNCTION(BlueprintPure, Category="UI|ViewModels")
    UVMUnit* GetUnitVM() const { return UnitVM; }
};
