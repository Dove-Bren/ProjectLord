// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LandscapePainter.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UENUM(BlueprintType)
enum class ELandscapePaint : uint8
{
    Foundation,

    MAX UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ELandscapePaint, ELandscapePaint::MAX);

UCLASS(BlueprintType)
class PROJECTLORD_API ALandscapePainter : public AActor
{
    GENERATED_BODY()

public:
    ALandscapePainter();

    static ALandscapePainter* GetLandscapePainter(const UObject* WorldContext);

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable)
    void Paint(ELandscapePaint Type, FVector Location, float Radius);


protected:

    UPROPERTY()
    TMap<ELandscapePaint, UTextureRenderTarget2D*> DynamicLandscapeLayers;

    UPROPERTY(EditInstanceOnly, Category = "LandscapePainting")
    TObjectPtr<UMaterialInterface> BrushMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> BrushInstance;

    UPROPERTY(VisibleInstanceOnly, Category = "LandscapePainting")
    int WorldWidth = 1024 * 100;

    UPROPERTY(VisibleInstanceOnly, Category = "LandscapePainting")
    int WorldHeight = 1024 * 100;

    UPROPERTY(VisibleInstanceOnly, Category = "LandscapePainting")
    int WorldOffsetX = -(1024 * 100) / 2;

    UPROPERTY(VisibleInstanceOnly, Category = "LandscapePainting")
    int WorldOffsetY = -(1024 * 100) / 2;

    // How many world units (cm) per pixel?
    UPROPERTY(EditInstanceOnly, Category = "LandscapePainting")
    float LayerScale = 16;

    UPROPERTY(VisibleInstanceOnly, Category = "LandscapePainting")
    bool bReady = false;

    void SetupLayers();

    static FName BrushParam_Location;
    static FName BrushParam_Radius;


};
