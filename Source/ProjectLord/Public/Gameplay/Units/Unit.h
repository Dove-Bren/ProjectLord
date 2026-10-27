// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GameplayEffect.h"
#include "GameplayTagAssetInterface.h"

#include "Gameplay/GameTeam.h"
#include "Gameplay/Units/UnitTypes.h"
#include "Gameplay/Combat/Ability/AbilityEnums.h"
#include "Gameplay/Combat/Ability/CombatAbility.h" // Including instead of forward declared to make dropdowns populate right

#include "Unit.generated.h"

class AUnitController;
class ULordAbilitySystemComponent;
class UCombatComponent;
class ULordUnitAttributeSet;
class UCombatAttributeSet;
class UVMUnit;
class UWidgetComponent;
class USelectionComponent;
class UUnitType;
class UAnimMontage;
class UStaticMeshComponent;
class AGameTeamState;

struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDeath, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitFinalDeath, AUnit*, Unit);

UCLASS(Blueprintable, meta = (PrioritizeCategories = "Unit Combat Selection"))
class PROJECTLORD_API AUnit : public ACharacter, public IGameplayTagAssetInterface
{
    GENERATED_BODY()

public:
    AUnit();

    UPROPERTY(BlueprintAssignable, Category = "Unit")
    FOnUnitDeath OnUnitDeath;

    UPROPERTY(BlueprintAssignable, Category = "Unit")
    FOnUnitFinalDeath OnUnitFinalDeath;

    UFUNCTION(BlueprintPure, Category = "Unit")
    AUnitController* GetUnitController() const;

    UFUNCTION(BlueprintPure, Category = "Unit")
    UUnitType* GetUnitType() const { return UnitType; }

    UFUNCTION(BlueprintPure, Category = "Unit")
    virtual FText GetUnitName() const { return (ensure(GetUnitType())) ? GetUnitType()->UnitName : FText::GetEmpty(); }

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const { return Team; }

    UFUNCTION(BlueprintCallable)
    virtual void Server_SetTeam(EGameTeam InTeam);

    UFUNCTION(BlueprintPure)
    AGameTeamState* GetTeamState() const;

    UFUNCTION(BlueprintPure)
    bool IsDead() const;

    UFUNCTION(BlueprintPure)
    bool IsAlive() const { return !IsDead(); }

    UFUNCTION(BlueprintCallable, Category = "Actor")
    void FaceActor(AActor* OtherActor);

    UFUNCTION(BlueprintCallable, Category = "Actor")
    virtual void Server_HandleUnitRecruited();

    UFUNCTION(BlueprintPure)
    UCombatAttributeSet* GetCombatAttributeSet() const { return CombatAttributeSet; }

    UFUNCTION(BlueprintPure)
    UAnimMontage* GetAnimForAbilityType(EAbilityAnimType AbilityType) const;


public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void ServerTick(float DeltaSeconds);
    virtual void ClientTick(float DeltaSeconds);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ServerTick"))
    void BP_ServerTick(float DeltaSeconds);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ClientTick"))
    void BP_ClientTick(float DeltaSeconds);

    // Tag Interface
    virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
    virtual bool HasMatchingGameplayTag(FGameplayTag Tag) const override;
    virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
    virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
    // End tag interface

protected:

    UFUNCTION(BlueprintNativeEvent)
    void OnDeath();

    // When a unit is removed (no grave, no lingering) permanently.
    // For heroes and some monsters, this is after thier gravestone eventually fades away
    UFUNCTION(BlueprintCallable)
    virtual void OnFinalDeath();

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnFinalDeath"))
    void BP_OnFinalDeath();

    virtual void PlayDeathAnimation();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void RefreshHealthbarVisibility();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Abilities")
    TObjectPtr<ULordAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Creature")
    TObjectPtr<UStaticMeshComponent> SelectionRingComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Definition")
    TObjectPtr<UUnitType> UnitType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Team, Category = "Unit")
    EGameTeam Team;

    UPROPERTY(EditDefaultsOnly, Category = "Unit|Definition|Attributes", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.AttributeBaseValue"))
    TObjectPtr<UDataTable> ClassAttributeDefaults;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

    UPROPERTY(EditInstanceOnly,BlueprintReadWrite, Category = "Selection")
    TObjectPtr<USelectionComponent> SelectionComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Definition")
    TMap<EAbilityAnimType, UAnimMontage*> AbilityAnimations;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Definition")
    TObjectPtr<UAnimMontage> DeathAnimation;

    virtual void Sided_RegisterAttributes();
    virtual void Server_SetupBaseAttributes();
    virtual void Client_SetupSelectionData(USelectionComponent* SelectionComponent);

    virtual void Client_InitUnitVM();
    virtual UVMUnit* Client_ConstructUnitVM();

    UFUNCTION()
    virtual void OnRep_Team(EGameTeam OldTeam);

private:
    void AddHealthbarWidget();

    UPROPERTY()
    TObjectPtr<UWidgetComponent> HealthbarWidgetComponent;

    UPROPERTY()
    TObjectPtr<UVMUnit> UnitVM;

    UFUNCTION()
    void Sided_HandleDeath();

    UFUNCTION()
    void Server_HandleAttack(AActor* Target, UCombatComponent* TargetComponent);

public:
    UFUNCTION(BlueprintPure, Category="UI|ViewModels")
    UVMUnit* GetUnitVM() const { return UnitVM; }
};
