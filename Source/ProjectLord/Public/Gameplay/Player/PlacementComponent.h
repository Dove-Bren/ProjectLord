// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PlacementComponent.generated.h"

class APlacementActor;
class UBuildingType;
class ABuilding;
class UPlaceBuildingPurchase;

USTRUCT()
struct PROJECTLORD_API FPlaceBuildingOperation
{
    GENERATED_BODY()

public:

    UPROPERTY()
    TObjectPtr<const UBuildingType> BuildingType;

    UPROPERTY()
    FVector At = {};

    UPROPERTY()
    int PlaceCost = 0;

    UPROPERTY()
    TObjectPtr<const UPlaceBuildingPurchase> PurchaseAction;

    TObjectPtr<ABuilding> PlacedBuilding;
};

UCLASS()
class PROJECTLORD_API UPlacementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlacementComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void StartPlacing(const UBuildingType* Type, int Cost, const UPlaceBuildingPurchase* Action);
    void CancelPlacing();
    TOptional<FPlaceBuildingOperation> AttemptToPlace();
    bool CanPlace() const;

    bool IsPlacing() const { return !!PlaceType; }
    const UBuildingType* GetPlaceType() const { return PlaceType; }
    const UPlaceBuildingPurchase* GetPlaceAction() const { return PlaceAction; }
    int GetPlaceCost() const { return PlaceCost; }

protected:
    FVector GetPlaceLocation() const;
    APlacementActor* SpawnPlacementActor();

private:

    UPROPERTY()
    TObjectPtr<const UBuildingType> PlaceType;

    UPROPERTY()
    TObjectPtr<const UPlaceBuildingPurchase> PlaceAction;

    UPROPERTY()
    int PlaceCost;

    UPROPERTY()
    TObjectPtr<APlacementActor> ShadowActor;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<APlacementActor> PlacementActorType;

};
