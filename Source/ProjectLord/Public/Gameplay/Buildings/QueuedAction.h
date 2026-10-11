// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GameGood.h"
#include "QueuedAction.generated.h"

class UTexture2D;
class UUnitType;
class AGoodBuilding;
class UPlayerSpell;

USTRUCT(BlueprintType)
struct PROJECTLORD_API FQueuedAction
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building|Queue|Action")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building|Queue|Action")
    int RefundGold;

    UPROPERTY()
    TObjectPtr<class UQueuedActionExecution> Execution;

};

UCLASS(BlueprintType, Abstract)
class PROJECTLORD_API UQueuedActionExecution : public UObject
{
    GENERATED_BODY()

public:

    virtual void Perform(AGoodBuilding* Building) {}

    virtual bool IsSupportedForNetworking() const override { return true; }
};

UCLASS(BlueprintType)
class PROJECTLORD_API UQueuedGoodAction : public UQueuedActionExecution
{
    GENERATED_BODY()

public:

    static FQueuedAction Make(UObject* Outer, int InGold, FGoodOffer InOffer);

    UFUNCTION(BlueprintPure, Category = "Building|Queue|Action")
    FGoodOffer GetGood() const { return Good; }

    virtual void Perform(AGoodBuilding* Building) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Building|Queue|Action")
    FGoodOffer Good;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UQueuedSpellAction : public UQueuedActionExecution
{
    GENERATED_BODY()

public:

    static FQueuedAction Make(UObject* Outer, int InGold, UPlayerSpell* InSpell);

    UFUNCTION(BlueprintPure, Category = "Building|Queue|Action")
    UPlayerSpell* GetSpell() const { return Spell; }

    virtual void Perform(AGoodBuilding* Building) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Building|Queue|Action")
    TObjectPtr<UPlayerSpell> Spell;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UQueuedRecruitAction : public UQueuedActionExecution
{
    GENERATED_BODY()

public:

    static FQueuedAction Make(UObject* Outer, int InGold, UUnitType* InType);

    UFUNCTION(BlueprintPure, Category = "Building|Queue|Action")
    UUnitType* GetRecruitType() const { return RecruitType; }

    virtual void Perform(AGoodBuilding* Building) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Building|Queue|Action")
    TObjectPtr<UUnitType> RecruitType;
};
