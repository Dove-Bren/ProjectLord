// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Gameplay/Combat/Ability/CombatAbility.h"
#include "GameGood.generated.h"

class UTexture2D;
class UHeroItemDef;
class UUnitType;
class UVMGameGood;

// A "Good" that can be sold and purchased to heroes.
// Was going to name this "HeroGood" but that felt confusing -- especially
// to live outside the Units/ directory.
// Note that goods are basically either an item, or an ability
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

	UFUNCTION(BlueprintPure, Category = "Game Good")
	UVMGameGood* GetOrCreateViewModel();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR  


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good", meta = (EditCondition = "Ability == nullptr && Item == nullptr"))
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good", meta = (EditCondition = "Ability == nullptr && Item == nullptr"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good", meta = (EditCondition = "Ability == nullptr && Item == nullptr"))
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good", meta = (EditCondition = "Ability == nullptr"))
	TObjectPtr<const UHeroItemDef> Item;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good", meta = (EditCondition = "Item == nullptr"))
	TSubclassOf<UCombatAbility> Ability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Good")
	TObjectPtr<const UUnitType> UnitTypeRestriction;

	UPROPERTY(BlueprintReadOnly, Category = "Game Good")
	TObjectPtr<UVMGameGood> ViewModel;
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
