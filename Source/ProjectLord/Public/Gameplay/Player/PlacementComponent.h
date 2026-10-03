// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PlacementComponent.generated.h"

class APlacementActor;
class UBuildingType;
class ABuilding;

UCLASS()
class PROJECTLORD_API UPlacementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlacementComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void StartPlacing(const UBuildingType* Type, int Cost);
    void CancelPlacing();
    ABuilding* AttemptToPlace();
    bool CanPlace() const;

    bool IsPlacing() const { return !!PlaceType; }
    const UBuildingType* GetPlaceType() const { return PlaceType; }
    int GetPlaceCost() const { return PlaceCost; }

protected:
    FVector GetPlaceLocation() const;
    APlacementActor* SpawnPlacementActor();

private:

    UPROPERTY()
    TObjectPtr<const UBuildingType> PlaceType;

    UPROPERTY()
    int PlaceCost;

    UPROPERTY()
    TObjectPtr<APlacementActor> ShadowActor;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<APlacementActor> PlacementActorType;

};
