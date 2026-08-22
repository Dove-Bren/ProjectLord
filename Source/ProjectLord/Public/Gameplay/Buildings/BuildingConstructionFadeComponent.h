// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingConstructionFadeComponent.generated.h"

class ABuilding;
class UMaterialInstanceDynamic;

UCLASS()
class PROJECTLORD_API UBuildingConstructionFadeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBuildingConstructionFadeComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable)
    void Enable();


protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    int IdleTickCount;

    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> BuildingFadeMaterials;

    UFUNCTION(BlueprintCallable)
    void UpdateMesh(float ConstructionProgress);

    UFUNCTION(BlueprintPure)
    ABuilding* GetBuilding() const;

    void SetupMaterials();

};
