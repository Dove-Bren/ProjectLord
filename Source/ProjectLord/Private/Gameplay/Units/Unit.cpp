// Copyright (c) Skyler Manzanares. All Rights Reserved.

#include "Gameplay/Units/Unit.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Gameplay/Units/AI/UnitController.h"

AUnit::AUnit() : ACharacter()
{
    // Set up defaults
    WanderRadius = 1000;
    Sight = 500;
    AttackRange = 50;

    Level = 1;
    Team = EUnitTeam::Monster;

    // Adjust character stuff
    GetCapsuleComponent()->InitCapsuleSize(22.0f, 50.0f);
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

AUnitController* AUnit::GetUnitController() const
{
    return Cast<AUnitController>(GetController());
}

bool AUnit::IsCloseEnoughToAttack(const AUnit* OtherUnit) const
{
    return this->GetDistanceTo(OtherUnit) <= AttackRange;
}

bool AUnit::CanAttack_Implementation() const
{
    return !bIsAttacking;
}

void AUnit::DamageUnit_Implementation(FAttackDamage InDamage)
{
    if (IsAlive())
    {
        const int Defense = GetDefenseFor(InDamage.DamageType);
        const int TotalDamage = FMath::Clamp(InDamage.Amount - Defense, 0, MaxHealth);
        Health = FMath::Clamp(Health - TotalDamage, 0, MaxHealth); // TODO setter?

        if (IsDead())
        {
            OnDeath();
        }
    }
}

void AUnit::AttackUnit_Implementation(AUnit* TargetUnit)
{
    bIsAttacking = true;

    // For now, just wait some time and then do the damage
    TWeakObjectPtr<AUnit> WeakTarget(TargetUnit);
    GetWorldTimerManager().SetTimer(TempAttackTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this, WeakTarget]() {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Unit attack complete; dealing 10 damage"));
            }

            if (WeakTarget.IsValid())
            {
                FAttackDamage Damage;
                Damage.DamageType = EDamageType::Slash;
                Damage.Amount = 10;
                WeakTarget->DamageUnit(Damage);
            }
            bIsAttacking = false;
        }), 3.0f, false);

}

void AUnit::OnDeath_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Unit did not override OnDeath!"));
    this->Destroy();
}

FDefenses AUnit::GetDefenses_Implementation() const
{
    return BaseDefense;
}
