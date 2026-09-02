// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GameTeam.h"
#include "FogOfWar.generated.h"

class UMaterialInterface;

// Actor placed in world to turn on for of war.
// Contains settings for how the fog of war should work.
UCLASS(BlueprintType, meta=(PrioritizeCategories="FogOfWar"))
class PROJECTLORD_API AFogOfWar : public AActor
{
    GENERATED_BODY()

public:
    
    // Scale of the Fog of War underlying map.
    // Each unit here means that many world units will be represented by 1 FoW texel.
    UPROPERTY(EditInstanceOnly, Category = "FogOfWar")
    int MapScale = 16;

    // Whether the map size should be determined automatically be checking the bounds
    // of the landscape. Not this only works if the landscape is not set up to Stream.
    UPROPERTY(EditInstanceOnly, Category = "FogOfWar")
    bool bAutoMapSize = false;

    UPROPERTY(EditInstanceOnly, Category = "FogOfWar", meta = (EditCondition = "!bAutoMapSize", EditConditionHides))
    double MapWidth = 5000;

    UPROPERTY(EditInstanceOnly, Category = "FogOfWar", meta = (EditCondition = "!bAutoMapSize", EditConditionHides))
    double MapHeight = 5000;

    UPROPERTY(EditInstanceOnly, Category = "FogOfWar", meta = (EditCondition = "!bAutoMapSize", EditConditionHides))
    FVector MapMinPoint = FVector(0, 0, 0);

    UPROPERTY(EditInstanceOnly, Category = "FogOfWar")
    UMaterialInterface* BrushMaterial;

    UPROPERTY(EditInstanceOnly, Category = "FogOfWar")
    UMaterialInterface* WorldFogMaterial;

    UPROPERTY(EditInstanceOnly, Category = "FogOfWar")
    EGameTeam DefaultTeamToDisplay = EGameTeam::Player1;

    virtual void BeginPlay() override;

};
