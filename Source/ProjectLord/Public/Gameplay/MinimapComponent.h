// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/GameTeam.h"
#include "MinimapComponent.generated.h"

class UTexture2D;
class AMinimap;
class UVMMinimapIcon;

UCLASS(BlueprintType)
class PROJECTLORD_API UMinimapComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMinimapComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void RegisterWithMinimap();

    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void UnregisterFromMinimap();

    UFUNCTION(BlueprintPure, Category = "Minimap")
    UTexture2D* GetMapIcon() const { return MapIcon; }

    UFUNCTION(BlueprintPure, Category = "Minimap")
    bool IsVisible() const;

    UFUNCTION(BlueprintPure, Category = "Minimap")
    FVector GetWorldLocation() const;

    UFUNCTION(BlueprintPure, Category = "Minimap")
    EGameTeam GetTeam() const { return Team; }

    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void SetTeam(EGameTeam InTeam);

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Minimap")
    TObjectPtr<UVMMinimapIcon> ViewModel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    TObjectPtr<UTexture2D> MapIcon;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Minimap")
    EGameTeam Team;

    UPROPERTY(EditDefaultsOnly, Category = "Minimap")
    bool bAutoRegister;

    bool bRegistered;
    
};
