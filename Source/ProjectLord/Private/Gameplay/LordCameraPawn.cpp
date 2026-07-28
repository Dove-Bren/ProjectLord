// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordCameraPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"

ALordCameraPawn::ALordCameraPawn()
{
    auto CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SetRootComponent(CollisionSphere);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));

    bUseControllerRotationYaw = true;

    SpringArm->SetupAttachment(RootComponent);
    SpringArm->SetRelativeRotation(FRotator(-90.0f, 0, 0));
    SpringArm->TargetArmLength = 700;
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritRoll = false;

    Camera->SetupAttachment(SpringArm);
    Camera->SetRelativeRotation(FRotator(30.0f, 0, 0));

    FloatingMovement->Acceleration = 8000;
    FloatingMovement->Deceleration = 10000;
}
