// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"
#include "Gameplay/Combat/Ability/AbilityEnums.h"
#include "Gameplay/GameTeam.h"

#include "PlayerSpell.generated.h"

class UTexture2D;
class UCombatComponent;
class ALordPlayerController;
class UVMPlayerSpell;

UCLASS(Blueprintable)
class PROJECTLORD_API UPlayerSpell : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPlayerSpell();
    virtual void PostInitProperties() override;

    const FText& GetSpellName() const { return SpellName; }
    const FText& GetSpellDescription() const { return SpellDescription; }
    UTexture2D* GetSpellIcon() const { return SpellIcon; }
    ESpellTargetType GetTargetType() const { return TargetType; }
    bool CanTargetBuildings() const { return bCanTargetBuildings; }
    TSubclassOf<UGameplayAbility> GetCastAbility() const { return CastAbility; }


    UFUNCTION(BlueprintPure, Category = "Spell")
    int GetBaseGoldCost() const { return BaseGoldCost; }

    UFUNCTION(BlueprintPure, Category = "Spell")
    virtual int GetGoldCostAt(const ALordPlayerController* Caster, FVector At) const { return GetBaseGoldCost(); }

    UFUNCTION(BlueprintPure, Category = "Spell")
    virtual bool CanCastAt(const ALordPlayerController* Caster, FVector At) const { return true; }

    UFUNCTION(BlueprintPure, Category = "Spell")
    virtual bool CanCastOn(const ALordPlayerController* Caster, UCombatComponent* Target) const;

    UFUNCTION(BlueprintPure, Category = "Spell")
    bool HasValidTarget(const ALordPlayerController* Caster) const;

    UFUNCTION(BlueprintCallable, Category = "Spell")
    UVMPlayerSpell* MakeViewModel(const UObject* WorldContextObject);

    bool CanCast(const ALordPlayerController* Caster, FVector At, UCombatComponent* Target) const;
    bool AttemptCast(ALordPlayerController* Source, FVector At, UCombatComponent* Target);

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell|Definition")
    FText SpellName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell|Definition")
    FText SpellDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell|Definition")
    TObjectPtr<UTexture2D> SpellIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell|Definition")
    TSubclassOf<UGameplayAbility> CastAbility;

    UPROPERTY(EditDefaultsOnly, Category = "Spell|Definition")
    int BaseGoldCost;

    UPROPERTY(EditDefaultsOnly, Category = "Spell|Definition")
    ESpellTargetType TargetType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell|Definition")
    bool bCanTargetBuildings;

    bool CheckCost(const ALordPlayerController* Source, FVector At) const;
    void ApplyCost(ALordPlayerController* SourceASC, FVector At) const;
};
