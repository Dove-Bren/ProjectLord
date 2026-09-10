// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "LordCameraPawn.generated.h"

class AActor;
class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;

UCLASS()
class PROJECTLORD_API ALordCameraPawn : public APawn
{
    GENERATED_BODY()

public:
    ALordCameraPawn();

    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "Focus")
    void SetFocusedActor(const AActor* Actor);

    UFUNCTION(BlueprintCallable, Category = "Focus")
    void ClearFocusedActor();

    UFUNCTION(BlueprintCallable, Category = "Focus")
    void PanTo(FVector WorldPosition);

    UFUNCTION(BlueprintCallable, Category = "Focus")
    void ClearPanTarget();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UFloatingPawnMovement* FloatingMovement;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Focus")
    TObjectPtr<const AActor> FocusedActor;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Focus")
    TOptional<FVector> PanTarget;

    void FocusTick(float DeltaSeconds);
};
