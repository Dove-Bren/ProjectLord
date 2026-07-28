// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "LordCameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;

UCLASS()
class PROJECTLORD_API ALordCameraPawn : public APawn
{
    GENERATED_BODY()

public:
    ALordCameraPawn();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UFloatingPawnMovement* FloatingMovement;
};
