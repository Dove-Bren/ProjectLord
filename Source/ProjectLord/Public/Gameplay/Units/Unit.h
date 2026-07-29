// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"

#include "Gameplay/Combat/CombatTypes.h"
#include "Gameplay/Units/UnitTypes.h"
#include "Gameplay/Combat/Ability/UnitAbility.h" // Including instead of forward declared to make dropdowns populate right

#include "Unit.generated.h"

class AUnitController;
class UAbilitySystemComponent;
class UCombatComponent;
class ULordUnitAttributeSet;
class UCombatAttributeSet;
class UVMUnit;

UCLASS(Blueprintable)
class PROJECTLORD_API AUnit : public ACharacter
{
    GENERATED_BODY()

public:
    AUnit();


    UFUNCTION(BlueprintPure)
    AUnitController* GetUnitController() const;

    UFUNCTION(BlueprintPure)
    EUnitTeam GetTeam() const { return Team; }

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
    EUnitTeam Team;

    UPROPERTY(EditDefaultsOnly, Category = "Attributes", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.UnitBaseAttributes"))
    TObjectPtr<UDataTable> ClassAttributeDefaults;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

    virtual void RegisterAttributes();
    virtual void SetupBaseAttributes();

private:
    UPROPERTY()
    TObjectPtr<UVMUnit> UnitVM;

    UFUNCTION()
    void HandleDeath();

    UFUNCTION()
    void HandleAttack(AActor* Target, UCombatComponent* TargetComponent);

public:
    UFUNCTION(BlueprintCallable, Category="UI|ViewModels")
    UVMUnit* GetUnitVM();
};
