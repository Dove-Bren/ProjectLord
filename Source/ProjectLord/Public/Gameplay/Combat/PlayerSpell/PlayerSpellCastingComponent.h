// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSpellCastingComponent.generated.h"

class UPlayerSpell;
class ALordPlayerController;
class UVMPlayerSpellCasting;
class UCombatComponent;

USTRUCT()
struct PROJECTLORD_API FPlayerSpellCastOperation
{
    GENERATED_BODY()

public:

    UPROPERTY()
    TObjectPtr<UPlayerSpell> Spell;

    UPROPERTY()
    FVector TargetLocation = {};

    UPROPERTY()
    TObjectPtr<UCombatComponent> TargetComponent;

};

DECLARE_MULTICAST_DELEGATE(FOnSpellChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpellCast, UPlayerSpell* /*Spell*/);

UCLASS(BlueprintType)
class PROJECTLORD_API UPlayerSpellCastingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlayerSpellCastingComponent();

    FOnSpellChanged OnSpellChanged;
    FOnSpellCast OnSpellCast;

    void StartCasting(UPlayerSpell* Spell);
    void CancelCasting();
    bool CanCast() const;
    TOptional<FPlayerSpellCastOperation> AttemptToCast();

    bool IsCasting() const { return !!CurrentSpell; }
    int GetCastCost() const;

    UFUNCTION(BlueprintCallable, Category = "SpellCasting")
    UVMPlayerSpellCasting* GetViewModel() const { return ViewModel; }

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:

    ALordPlayerController* GetPlayerController() const;
    bool CanAfford(int CurrentCost) const;
    bool CanAfford() const { return CanAfford(GetCastCost()); }

    UPROPERTY()
    TObjectPtr<UPlayerSpell> CurrentSpell;

    UPROPERTY()
    TObjectPtr<UVMPlayerSpellCasting> ViewModel;



};
