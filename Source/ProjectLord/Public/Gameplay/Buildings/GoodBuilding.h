// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/ResidentialBuilding.h"

#include "GoodBuilding.generated.h"

class UGameGood;
struct FGoodOffer;

// A building that can have goods
UCLASS(Blueprintable)
class PROJECTLORD_API AGoodBuilding : public AResidentialBuilding
{
    GENERATED_BODY()

public:
    AGoodBuilding();

    UFUNCTION(BlueprintCallable)
    void AddGoodOffer(FGoodOffer InOffer);

    UFUNCTION(BlueprintPure)
    UBuildingActionQueueComponent* GetQueueComponent() const { return QueueComponent; }

    UFUNCTION(BlueprintPure)
    const TArray<FGoodOffer>& GetGoods() const { return Goods; }

    UFUNCTION(BlueprintPure)
    bool HasGood(UGameGood* GoodType) const;

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Building|Definition")
    TArray<FGoodOffer> DefaultGoods;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building")
    TObjectPtr<UBuildingActionQueueComponent> QueueComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Contents")
    TArray<FGoodOffer> Goods;

    virtual void SetupBaseGoods();

    UFUNCTION()
    void OnQueueActionReady(UQueuedAction* Action);

public:

    virtual void BeginPlay() override;
};
