// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/Unit.h"

#include "Creature.generated.h"

class AResidentialBuilding;
class UCombatAttributeSet;
class UCreatureAttributeSet;
class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

// A Unit that moves
UCLASS(Blueprintable, meta = (PrioritizeCategories = "Unit Combat Selection Creature"))
class PROJECTLORD_API ACreature : public AUnit
{
    GENERATED_BODY()

public:
    ACreature();

    // Old Depreceted
    UUnitType* GetCreatureType() const { return GetUnitType(); }
    AResidentialBuilding* GetHomeBuilding() const { return HomeBuilding.IsValid() ? HomeBuilding.Get() : nullptr; }

    UFUNCTION(BlueprintPure)
    bool HasBuilding() const { return !!GetHomeBuilding(); }

    UFUNCTION(BlueprintCallable)
    void SetHomeBuilding(AResidentialBuilding* Building);

    UFUNCTION(BlueprintPure)
    UCreatureAttributeSet* GetCreatureAttributeSet() const { return CreatureAttributeSet; }


protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Creature")
    TWeakObjectPtr<AResidentialBuilding> HomeBuilding;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UCreatureAttributeSet> CreatureAttributeSet;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Creature")
    TObjectPtr<UStaticMeshComponent> GraveComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Creature")
    float DeadTime;

    virtual void RegisterAttributes() override;

    virtual void OnDeath_Implementation() override;
    virtual void OnFinalDeath() override;

    // Award gold evenly amonsts nearby heroes.
    // Returns true if any heroes got gold
    UFUNCTION(BlueprintCallable)
    virtual bool AwardGoldToNearbyHeroes(int Gold);

    virtual bool ShouldHaveGravestone() const { return false; }

    // Grave BP interaction
    UFUNCTION(BlueprintImplementableEvent, Category = "Creature|Death")
    void OnBeginFadeOutBody();

    UFUNCTION(BlueprintImplementableEvent, Category = "Creature|Death")
    void OnUpdateFadeOutBody(float Percent);

    UFUNCTION(BlueprintImplementableEvent, Category = "Creature|Death")
    void OnFinishFadeOutBody();

    UFUNCTION(BlueprintImplementableEvent, Category = "Creature|Death")
    void OnBeginFadeInGrave();

    UFUNCTION(BlueprintImplementableEvent, Category = "Creature|Death")
    void OnBeginFadeOutGrave();

    UFUNCTION(BlueprintImplementableEvent, Category = "Creature|Death")
    void OnFinishFadeInGrave();

    UFUNCTION(BlueprintImplementableEvent, Category = "Creature|Death")
    void OnFinishFadeOutGrave();

    UFUNCTION(BlueprintImplementableEvent, Category = "Creature|Death")
    void OnUpdateFadeGrave(float Percent);

    void StartDeathFade();
    void StartFadingInGrave();
    void StartFadingOutGrave();
    void FadeTick();

private:

    // Fading
    float FadingBodyStartTime;
    float FadingGraveStartTime;
    TArray<UMaterialInstanceDynamic*> BodyFadeMaterials;
    TArray<UMaterialInstanceDynamic*> GraveFadeMaterials;
    FTimerHandle GraveTimer;
    bool bFading = false;

    void CreateFadeMaterials();
    void FadeBodyTo(float Percent);
    void FadeGraveTo(float Percent);
    

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void Tick(float TimeElapsed) override;


};
