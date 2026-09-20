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

void ALordCameraPawn::SetFocusedActor(const AActor* Actor)
{
    FocusedActor = Actor;
    ClearPanTarget();
}

void ALordCameraPawn::ClearFocusedActor()
{
    FocusedActor = nullptr;
}

void ALordCameraPawn::PanTo(FVector WorldPosition)
{
    PanTarget = WorldPosition;
}

void ALordCameraPawn::ClearPanTarget()
{
    PanTarget.Reset();
}

void ALordCameraPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    FocusTick(DeltaSeconds);
}

void ALordCameraPawn::FocusTick(float DeltaSeconds)
{
    if (IsValid(FocusedActor) || PanTarget.IsSet())
    {
        const FVector TargetPos = (IsValid(FocusedActor) ? FocusedActor->GetActorLocation() : PanTarget.GetValue())
                        * FVector(1, 1, 0); // Always focus on z=0
        const FVector MyPos = GetActorLocation();
        const double DistSqr = FVector::DistSquaredXY(MyPos, TargetPos);
        
        // If really close OR too far, just jump there
        if (DistSqr < 50 * 50 || DistSqr > 100 * 100)
        {
            SetActorLocation(TargetPos);
            GetMovementComponent()->StopMovementImmediately();

            // Clear pan target if that's where we were headed
            ClearPanTarget();
        }
        else
        {
            FVector Direction = (TargetPos - MyPos);
            Direction.Normalize();
            AddMovementInput(Direction, FMath::Log2(DistSqr));
        }
    }
}
