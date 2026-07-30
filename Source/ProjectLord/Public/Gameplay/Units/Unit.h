// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"

#include "Gameplay/GameTeam.h"
#include "Gameplay/Units/UnitTypes.h"
#include "Gameplay/Combat/Ability/UnitAbility.h" // Including instead of forward declared to make dropdowns populate right

#include "Unit.generated.h"

class AUnitController;
class UAbilitySystemComponent;
class UCombatComponent;
class ULordUnitAttributeSet;
class UCombatAttributeSet;
class UVMUnit;
class UWidgetComponent;
class USelectionComponent;

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

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const { return Team; }

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
    EGameTeam Team;

    UPROPERTY(EditDefaultsOnly, Category = "Attributes", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.AttributeBaseValue"))
    TObjectPtr<UDataTable> ClassAttributeDefaults;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Selection")
    TObjectPtr<USelectionComponent> SelectionComponent;

    virtual void RegisterAttributes();
    virtual void SetupBaseAttributes();

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
