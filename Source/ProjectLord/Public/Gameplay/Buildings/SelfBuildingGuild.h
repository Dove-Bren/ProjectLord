// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/GuildBuilding.h"

#include "SelfBuildingGuild.generated.h"

struct FGameplayEffectSpecHandle;

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API ASelfBuildingGuild : public AGuildBuilding
{
    GENERATED_BODY()

public:
    
    virtual bool WantsRepair() const override { return false; }

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    double NextRepairTime;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SelfRepair")
    int HealthPerRepair;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SelfRepair")
    float RepairPeriod;

    FGameplayEffectSpecHandle RepairEffectHandle;

    virtual bool TimeToRepair(double Now) const;
    virtual void DoAutoRepair();
};
