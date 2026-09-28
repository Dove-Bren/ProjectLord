// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PlayerSpellGameplayAbility.generated.h"

class ALordPlayerController;
class UCombatComponent;
class UPlayerSpell;
struct FGameplayEventData;

UCLASS(Blueprintable)
class PROJECTLORD_API UPlayerSpellGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UPlayerSpellGameplayAbility();

    UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
    void ActivatePlayerSpell(ALordPlayerController* Player, FVector At, UCombatComponent* Target, UPlayerSpell* OriginalSpell);


    const TArray<FAbilityTriggerData>& GetAbilityTriggers() const { return AbilityTriggers; }
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    static FGameplayEventData MakeEventPayload(ALordPlayerController* Player, FVector At, UCombatComponent* Target, UPlayerSpell* OriginalSpell);
};
