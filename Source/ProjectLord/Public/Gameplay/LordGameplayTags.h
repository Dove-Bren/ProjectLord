// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

// Note: This is a helper for getting gameplay tags.
// Tags are actually defined in a GameplayTag ini file (Config/Tags/GameTags.ini)
// This helper is here to consolidate hardcoding those strings

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTags.h"

#include "LordGameplayTags.generated.h"

#define MAKE_TAG(Name, Path) \
    static FGameplayTag Name() { return FGameplayTag::RequestGameplayTag(TEXT(Path)); } 

UCLASS()
class PROJECTLORD_API ULordGameplayTags : public UBlueprintFunctionLibrary // Should have named it '...FunctionLibrary' ?
{
    GENERATED_BODY()

public:
    
    MAKE_TAG(AbilityTypeAttack, "Ability.Type.Attack");
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Ability|Type")
    static FGameplayTag K2_AbilityTypeAttack() { return AbilityTypeAttack(); }

    MAKE_TAG(DamageTypeMagic, "Damage.Type.Magic");
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Damage|Type")
    static FGameplayTag K2_DamageTypeMagic() { return DamageTypeMagic(); }

    MAKE_TAG(DamageTypeMelee, "Damage.Type.Melee");
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Damage|Type")
    static FGameplayTag K2_DamageTypeMelee() { return DamageTypeMelee(); }

    MAKE_TAG(DamageTypeRanged, "Damage.Type.Ranged");
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Damage|Type")
    static FGameplayTag K2_DamageTypeRanged() { return DamageTypeRanged(); }

    MAKE_TAG(UnitStateDead, "Unit.State.Dead");
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Unit|State")
    static FGameplayTag K2_UnitStateDead() { return UnitStateDead(); }

    MAKE_TAG(UnitStateAttacking, "Unit.State.Attacking");
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Unit|State")
    static FGameplayTag K2_UnitStateAttacking() { return UnitStateAttacking(); }

    MAKE_TAG(UnitStateInvulnerable, "Unit.State.Invulnerable");
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Unit|State")
    static FGameplayTag K2_UnitStateInvulnerable() { return UnitStateInvulnerable(); }

    MAKE_TAG(UnitStateVisiting, "Unit.State.Visiting");
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Unit|State")
    static FGameplayTag K2_UnitStateVisiting() { return UnitStateVisiting(); }

    MAKE_TAG(UnitClassifierBeast, "Unit.Classifier.Dead");
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Unit|Classifier")
    static FGameplayTag K2_UnitClassifierBeast() { return UnitClassifierBeast(); }
};
