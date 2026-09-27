// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/PlayerSpell/PlayerSpellCatalog.h"
#include "Engine/AssetManager.h"

#include "AssetHelper.h"
#include "LordLogging.h"
#include "Gameplay/Combat/PlayerSpell/PlayerSpell.h"

static FPrimaryAssetType PlayerSpellAssetType = FPrimaryAssetType(TEXT("PlayerSpell"));

void UPlayerSpellCatalog::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bLoadingSpells = true;
	GetPrimaryAssetsOfType<UPlayerSpell>(PlayerSpellAssetType, [this](TArray<UPlayerSpell*> Results) {
		PlayerSpells = Results;
		bLoadingSpells = false;
		UE_LOG(LordUnit, Log, TEXT("Loaded %d Player Spells"), PlayerSpells.Num());
	});
}

void UPlayerSpellCatalog::Deinitialize()
{
	Super::Deinitialize();

	ensure(!bLoadingSpells);

	UAssetManager& Manager = UAssetManager::Get();
	PlayerSpells.Empty();
	Manager.UnloadPrimaryAssetsWithType(PlayerSpellAssetType);
}
