// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GameGood.h"
#include "QueuedAction.generated.h"

class UTexture2D;
class UUnitType;
class ABuilding;

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UQueuedAction : public UObject
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Building|Queue|Action")
    UTexture2D* GetIcon() const { return Icon; }

    UFUNCTION(BlueprintPure, Category = "Building|Queue|Action")
    int GetRefundGold() const { return RefundGold; }

    UFUNCTION(BlueprintCallable, Category = "Building|Queue|Action")
    virtual void Perform(ABuilding* Building);

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Queue|Action")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building|Queue|Action")
    int RefundGold;

};

UCLASS(BlueprintType)
class PROJECTLORD_API UQueuedGoodAction : public UQueuedAction
{
    GENERATED_BODY()

public:

    void Init(int InGold, FGoodOffer InOffer);

    UFUNCTION(BlueprintPure, Category = "Building|Queue|Action")
    FGoodOffer GetGood() const { return Good; }

    virtual void Perform(ABuilding* Building) override;

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Queue|Action")
    FGoodOffer Good;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UQueuedRecruitAction : public UQueuedAction
{
    GENERATED_BODY()

public:

    void Init(int InGold, UUnitType* InType);

    UFUNCTION(BlueprintPure, Category = "Building|Queue|Action")
    UUnitType* GetRecruitType() const { return RecruitType; }

    virtual void Perform(ABuilding* Building) override;

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Queue|Action")
    TObjectPtr<UUnitType> RecruitType;
};
