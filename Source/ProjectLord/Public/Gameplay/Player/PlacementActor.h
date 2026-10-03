// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PlacementActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UStaticMesh;

UCLASS()
class PROJECTLORD_API APlacementActor : public AActor
{
    GENERATED_BODY()

public:
    APlacementActor();

    void SetMesh(UStaticMesh* Mesh);
    void SetLocation(FVector GroundLocation);
    bool HasOverlap() const;

    UFUNCTION(BlueprintImplementableEvent)
    void UpdatePlaceMaterial(bool bCanPlace);

protected:
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UBoxComponent> Collision;
};
