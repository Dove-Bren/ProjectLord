// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSpellCastingComponent.generated.h"

class UPlayerSpell;
class ALordPlayerController;

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
    bool AttemptToCast();

    bool IsCasting() const { return !!CurrentSpell; }
    int GetCastCost() const;


protected:

    UPlayerSpell* CurrentSpell;
    ALordPlayerController* GetPlayerController() const;

};
