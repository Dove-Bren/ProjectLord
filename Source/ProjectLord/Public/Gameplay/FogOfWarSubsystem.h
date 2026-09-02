// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/GameTeam.h"

#include "FogOfWarSubsystem.generated.h"

class UFogOfWarComponent;
class AFogOfWar;
class UTextureRenderTarget2D;

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

protected:

    TArray<TWeakObjectPtr<UFogOfWarComponent>> Components;

    TArray<TTuple<EGameTeam, FVector, float>> ExtraRevealSpots;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    TMap<EGameTeam, UTextureRenderTarget2D*> FogRenderTargets;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    TObjectPtr<UMaterialInstanceDynamic> FogRenderBrush;

    TMap<EGameTeam, TArray<TPair<FVector, float>>> WorkMap;

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

    float WorldXToSheetU(double WorldX) const
    {
        if (!IsActive()) return 0;
        return WorldXToSheetX(WorldX) / FogSheetWidth;
    }

    float WorldYToSheetV(double WorldY) const
    {
        if (!IsActive()) return 0;
        return WorldYToSheetY(WorldY) / FogSheetHeight;
    }

    constexpr static bool TeamHasFog(EGameTeam Team)
    {
        return EGameTeam::Monster != Team && EGameTeam::Neutral != Team;
    }

    static FName BrushParam_Location;
    static FName BrushParam_Radius;

};
