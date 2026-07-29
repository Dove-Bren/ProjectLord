// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UnitTypes.generated.h"

class UTexture2D;
class ACreature;

UCLASS(BlueprintType)
class PROJECTLORD_API UCreatureType : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Creature Type")
	FText CreatureName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Creature Type")
	FText CreatureDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Creature Type")
	TObjectPtr<UTexture2D> CreatureIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Creature Type")
	TSubclassOf<ACreature> CreatureClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Creature Type")
	int RecruitCost;

};
