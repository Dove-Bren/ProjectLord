// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Gameplay/Combat/Ability/CombatAbility.h"
#include "GameGood.generated.h"

class UTexture2D;
class UHeroItemDef;
class UUnitType;

// A "Good" that can be sold and purchased to heroes.
// Was going to name this "HeroGood" but that felt confusing -- especially
// to live outside the Units/ directory.
UCLASS(BlueprintType)
class PROJECTLORD_API UGameGood : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    
	UFUNCTION(BlueprintPure, Category = "Game Good")
	FText GetName() const { return Name; }

	UFUNCTION(BlueprintPure, Category = "Game Good")
	FText GetDescription() const { return Description; }

	UFUNCTION(BlueprintPure, Category = "Game Good")
	UTexture2D* GetIcon() const { return Icon; }

	UFUNCTION(BlueprintPure, Category = "Game Good")
	TSubclassOf<UCombatAbility> GetAbility() const { return Ability; }

	UFUNCTION(BlueprintPure, Category = "Game Good")
	const UHeroItemDef* GetItemDef() const { return Item; }

	UFUNCTION(BlueprintPure, Category = "Game Good")
	const UUnitType* GetUnitTypeRestriction() const { return UnitTypeRestriction; }



protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good")
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good")
	TObjectPtr<const UHeroItemDef> Item;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good")
	TSubclassOf<UCombatAbility> Ability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good")
	TObjectPtr<const UUnitType> UnitTypeRestriction;
};

USTRUCT(BlueprintType)
struct PROJECTLORD_API FGoodOffer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Good Offer")
	TObjectPtr<UGameGood> Good;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Good Offer")
	int GoldCost;
};
