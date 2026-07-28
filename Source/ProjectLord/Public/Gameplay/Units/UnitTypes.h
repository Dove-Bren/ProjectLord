// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UnitTypes.generated.h"

class UTexture2D;
class UStaticMesh;
class ACreature;
class ABuilding;

UENUM(BlueprintType)
enum class EUnitTeam : uint8
{
	Monster,
	Player1,
	Player2,
};

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

UCLASS(BlueprintType)
class PROJECTLORD_API UBuildingType : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	FText BuildingName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	FText BuildingDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	TObjectPtr<UTexture2D> BuildingIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	TObjectPtr<UStaticMesh> BuildingPlaceShape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	TSubclassOf<ABuilding> BuildingClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building Type")
	int BuildCost;

};
