// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorFadeComponent.generated.h"

class UMaterialInstanceDynamic;
class UCurveFloat;

USTRUCT()
struct FMaterialCacheRow
{
    GENERATED_BODY()
public:
    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> Materials;
};

DECLARE_MULTICAST_DELEGATE(FFadeComplete);

// Component that handles fading a static mesh out or in, or one out then another in.
// Can operate in a purely self-timed mode ("Fade out in 2 seconds") or a fully
// dynamic mode ("Fade, and ask me each tick what % we're at").
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class PROJECTLORD_API UActorFadeComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    using FGetDynamicOpacity = TFunction<float(void)>;

    UActorFadeComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    FFadeComplete FadeComplete;

    // Fade from 100% opacity to 0% opacity
    // Meshes must have a material with a matching [OpacityMaterialPropName] property to work.
    UFUNCTION(BlueprintCallable)
    void FadeOut(UPrimitiveComponent* Mesh, float Duration);

    UFUNCTION(BlueprintCallable)
    void FadeIn(UPrimitiveComponent* Mesh, float Duration);

    void FadeDynamic(UPrimitiveComponent* Mesh, FGetDynamicOpacity OpacityGetter);

    UFUNCTION(BlueprintCallable)
    void EndFade(float FinalProg = -1);

    static const FName OpacityMaterialPropName;

protected:

    enum class EFadeOperation : uint8
    {
        Idle,
        FadeOut,
        FadeIn,
        FadeDynamic,
    };

    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> FadeMaterials;

    UPROPERTY()
    TMap<UPrimitiveComponent*, FMaterialCacheRow> CachedMaterials;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UCurveFloat> FadeCurve;

    TOptional<FGetDynamicOpacity> FadeOpacityGetter;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    float FadeDuration;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    float FadeProgress;

    EFadeOperation FadeOperation;

    void SetupFor(UPrimitiveComponent*);

    UFUNCTION(BlueprintCallable)
    void FadeTo(float OpaquePercent);

};
