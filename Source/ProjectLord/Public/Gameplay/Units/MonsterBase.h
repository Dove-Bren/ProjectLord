// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/Creature.h"

#include "MonsterBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int);

UCLASS(Blueprintable)
class PROJECTLORD_API AMonsterBase : public ACreature
{
    GENERATED_BODY()

public:
    AMonsterBase();

    FOnGoldChanged OnGoldChanged;

    UFUNCTION(BlueprintPure)
    int GetGold() const { return Gold; }

protected:

    // How much treasure this monster is carrying
    UPROPERTY(EditDefaultsOnly, Category = "Resources", Meta=(ClampMin=0))
    int Gold;
    void SetGold(int InGold) { if (Gold != InGold) { Gold = InGold; OnGoldChanged.Broadcast(Gold); } }

    virtual void OnDeath_Implementation() override;
    virtual void SetupSelectionData(USelectionComponent* SelectionComponent) override;
    virtual bool ShouldHaveGravestone() const { return Gold > 0; }

};
