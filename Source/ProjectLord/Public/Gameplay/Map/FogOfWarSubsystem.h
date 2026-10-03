// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/GameTeam.h"

#include "FogOfWarSubsystem.generated.h"

class UFogOfWarComponent;
class AFogOfWar;
class UTextureRenderTarget2D;
class APostProcessVolume;

UCLASS(Blueprintable)
class PROJECTLORD_API UFogOfWarSubsystem : public UWorldSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UFogOfWarSubsystem();

    // Begin USubsystem
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    // End USubsystem

    // Begin FTickableGameObject
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual bool IsTickable() const override;
    // End FTickableGameObject

    UFUNCTION(BlueprintPure, Category = "Fog of War")
    bool IsActive() const { return bActive; }

    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    void Activate(AFogOfWar* WorldActor);

    UFUNCTION(BlueprintPure, Category = "Fog of War")
    bool IsInFog(EGameTeam Team, FVector Location) const;

    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    void AddRevealer(UFogOfWarComponent* Revealer);

    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    void RemoveRevealer(UFogOfWarComponent* Revealer);

    // Manually reveal a location instead of doing it automatically with a revealer
    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    void RevealFog(EGameTeam Team, FVector Location, float Radius);

    // Not meant to be used for gameplay logic.
    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    UTextureRenderTarget2D* GetTeamFogTarget(EGameTeam Team) const;

    // Makes the in-world fog display the current fog for the given team.
    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    void SetWorldFogForTeam(EGameTeam Team);

protected:

    TArray<TWeakObjectPtr<UFogOfWarComponent>> Components;

    TArray<TTuple<EGameTeam, FVector, float>> ExtraRevealSpots;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    TMap<EGameTeam, UTextureRenderTarget2D*> FogRenderTargets;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    TObjectPtr<UMaterialInstanceDynamic> FogRenderBrush;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    TObjectPtr<UMaterialInstanceDynamic> WorldFogVolumeBrush;

    TMap<EGameTeam, TArray<TPair<FVector, float>>> WorkMap;

    // CPU-only ugly version of where that's been revealed.
    // Used for gameplay queries, but not for display.
    TMap<EGameTeam, TUniquePtr<bool[]>> GameplayMaps;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    TObjectPtr<APostProcessVolume> WorldFogVolume;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    int FogSheetScale;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    int FogSheetWidth;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    int FogSheetHeight;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    FVector WorldMin;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    FVector WorldSize;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fog of War")
    bool bActive;

    void DoRevealPass();

    float WorldXToSheetX(double WorldX) const
    {
        if (!IsActive()) return 0;
        return (float)(FMath::Max(0, (WorldX - WorldMin.X)) / FogSheetScale);
    }

    float WorldYToSheetY(double WorldY) const
    {
        if (!IsActive()) return 0;
        return (float)(FMath::Max(0, (WorldY - WorldMin.Y)) / FogSheetScale);
    }

    float SheetXToSheetU(float SheetX) const
    {
        if (!IsActive()) return 0;
        return SheetX / FogSheetWidth;
    }

    float SheetYToSheetV(float SheetY) const
    {
        if (!IsActive()) return 0;
        return SheetY / FogSheetHeight;
    }

    constexpr static bool TeamHasFog(EGameTeam Team)
    {
        return EGameTeam::Monster != Team && EGameTeam::Neutral != Team;
    }

    static FName BrushParam_Texture;
    static FName BrushParam_Location;
    static FName BrushParam_Radius;
    static FName BrushParam_WorldMinOffset;
    static FName BrushParam_WorldSize;

};
