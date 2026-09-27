// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerSpellCatalog.generated.h"

class UPlayerSpell;

UCLASS(BlueprintType)
class PROJECTLORD_API UPlayerSpellCatalog: public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION(BlueprintPure, Category = "Player Spells")
	TArray<UPlayerSpell*> GetPlayerSpells() const { check(!bLoadingSpells); return PlayerSpells; }

private:
	TArray<UPlayerSpell*> PlayerSpells;

	bool bLoadingSpells;
};
