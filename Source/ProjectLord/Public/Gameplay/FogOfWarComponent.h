// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/GameTeam.h"
#include "FogOfWarComponent.generated.h"

class UFogOfWarSubsystem;

UCLASS(BlueprintType)
class PROJECTLORD_API UFogOfWarComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFogOfWarComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;

    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    void RegisterWithSubsystem();

    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    void UnregisterFromSubsystem();

    float GetRevealRadius() const { return RevealRadius; }
    FVector GetRevealLocation() const;
    EGameTeam GetTeam() const { return Team; }

    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    void SetRevealRadius(float Radius) { RevealRadius = Radius; }

    UFUNCTION(BlueprintCallable, Category = "Fog of War")
    void SetTeam(EGameTeam InTeam) { Team = InTeam; }

protected:
    
    static UFogOfWarSubsystem* GetSubsystem(const UObject* WorldContext);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog of War")
    float RevealRadius;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fog of War")
    EGameTeam Team;

    UPROPERTY(EditDefaultsOnly, Category = "Fog of War")
    bool bAutoRegister;

    bool bRegistered;
};
