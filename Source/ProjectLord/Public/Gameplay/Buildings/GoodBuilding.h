// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/ResidentialBuilding.h"

#include "GoodBuilding.generated.h"

class UGameGood;
struct FGoodOffer;

DECLARE_MULTICAST_DELEGATE(FOnBuildingGoodsChanged);

// A building that can have goods
UCLASS(Blueprintable)
class PROJECTLORD_API AGoodBuilding : public AResidentialBuilding
{
    GENERATED_BODY()

public:
    AGoodBuilding();

    FOnBuildingGoodsChanged OnBuildingGoodsChanged;

    UFUNCTION(BlueprintCallable)
    void AddGoodOffer(FGoodOffer InOffer);

    UFUNCTION(BlueprintPure)
    UBuildingActionQueueComponent* GetQueueComponent() const { return QueueComponent; }

    UFUNCTION(BlueprintPure)
    const TArray<FGoodOffer>& GetGoods() const { return Goods; }

    UFUNCTION(BlueprintPure)
    bool HasGood(UGameGood* GoodType, bool bCheckQueue = true) const;

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Building|Definition")
    TArray<FGoodOffer> DefaultGoods;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building")
    TObjectPtr<UBuildingActionQueueComponent> QueueComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Contents")
    TArray<FGoodOffer> Goods;

    virtual void SetupBaseGoods();
    virtual void SetupSelectionData(USelectionComponent* InSelectionComponent) override;

    UFUNCTION()
    void OnQueueActionReady(UQueuedAction* Action);

    virtual int GetResidentsInQueue(const UUnitType* Type) const override;

public:

    virtual void BeginPlay() override;
};
