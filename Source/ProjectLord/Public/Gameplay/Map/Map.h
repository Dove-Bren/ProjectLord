// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Map.generated.h"

class UMinimapComponent;
class UTextureRenderTarget2D;
class USceneCaptureComponent2D;

DECLARE_DELEGATE(FOnMapReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMinimapComponentsChange);

// Holds global 'map' (game level) data as well as minimap data.
UCLASS(BlueprintType)
class PROJECTLORD_API AMap : public AActor
{
    GENERATED_BODY()

public:
    AMap();

    static AMap* GetMap(const UObject* WorldContext);

    virtual void BeginPlay() override;

    void AddMapReadyHandler(const FOnMapReady& Callback)
    {
        if (IsReady())
        {
            Callback.ExecuteIfBound();
            return;
        }

        OnMapReady.Add(Callback);
    }

    UPROPERTY(BlueprintAssignable)
    FOnMinimapComponentsChange OnMinimapComponentsChange;

    UFUNCTION(BlueprintPure, Category = "Map")
    bool IsReady() const { return bReady; }

    UFUNCTION(BlueprintPure, Category = "Map")
    bool IsReadyChecked() const { check(IsReady()); return true; }

    UFUNCTION(BlueprintCallable, Category = "Map")
    void AddMinimapComponent(UMinimapComponent* Component);

    UFUNCTION(BlueprintCallable, Category = "Map")
    void RemoveMinimapComponent(UMinimapComponent* Component);

    UFUNCTION(BlueprintPure, Category = "Map")
    double GetMapWidth() const { return MapWidth; }

    UFUNCTION(BlueprintPure, Category = "Map")
    double GetMapHeight() const { return MapHeight; }

    UFUNCTION(BlueprintPure, Category = "Map")
    FVector GetMapMinPoint() const { return MapMinPoint; }

    UFUNCTION(BlueprintPure, Category = "Map")
    const TArray<UMinimapComponent*>& GetMinimapComponents() const { return MinimapComponents; }

    UFUNCTION(BlueprintPure, Category = "Map")
    UTextureRenderTarget2D* GetMapTexture() const { return MapTexture; }

    UFUNCTION(BlueprintCallable, Category = "Map")
    TArray<FVector2D> GetCameraBoundPoints() const;

    UFUNCTION(BlueprintCallable, Category = "Map|Scan")
    TArray<APlayerStart*> FindPlayerStarts() const;

protected:

    // Whether the map size should be determined automatically be checking the bounds
    // of the landscape. Not this only works if the landscape is not set up to Stream.
    UPROPERTY(EditInstanceOnly, Category = "Map")
    bool bAutoMapSize = false;

    UPROPERTY(EditInstanceOnly, Category = "Map", meta = (EditCondition = "!bAutoMapSize", EditConditionHides))
    double MapWidth = 5000;

    UPROPERTY(EditInstanceOnly, Category = "Map", meta = (EditCondition = "!bAutoMapSize", EditConditionHides))
    double MapHeight = 5000;

    UPROPERTY(EditInstanceOnly, Category = "Map", meta = (EditCondition = "!bAutoMapSize", EditConditionHides))
    FVector MapMinPoint = FVector(0, 0, 0);

    UPROPERTY(EditInstanceOnly, Category = "Map")
    int MapTextureLength = 512;

    UPROPERTY(EditInstanceOnly, Category = "Map")
    int MapTextureCaptureHeight = 500;

    UPROPERTY(VisibleInstanceOnly, Category = "Map")
    TArray<UMinimapComponent*> MinimapComponents;

    UPROPERTY(VisibleInstanceOnly, Category = "Map")
    TObjectPtr<UTextureRenderTarget2D> MapTexture;

    UPROPERTY(VisibleInstanceOnly, Category = "Map")
    TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;

    bool bReady;

    DECLARE_MULTICAST_DELEGATE(FOnMapReadyBroadcast);
    FOnMapReadyBroadcast OnMapReady;

    void CaptureMapTexture();
};
