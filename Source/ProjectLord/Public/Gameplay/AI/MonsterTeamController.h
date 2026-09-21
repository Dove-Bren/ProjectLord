// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Gameplay/GameTeam.h"

#include "MonsterTeamController.generated.h"

class UBehaviorTree;

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API AMonsterTeamController : public AAIController
{
    GENERATED_BODY()

public:
    AMonsterTeamController();

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintPure)
    EGameTeam GetTeam() const { return EGameTeam::Monster; }

    UFUNCTION(BlueprintPure)
    AGameTeamState* GetTeamState() const;

    UFUNCTION(BlueprintCallable)
    ABuilding* GetNearestPlayerBuilding(FVector From) const;

protected:

    void ActivateSpawners(int GameDays);

    UFUNCTION()
    void HandleDayChange(int GameDays);

};
