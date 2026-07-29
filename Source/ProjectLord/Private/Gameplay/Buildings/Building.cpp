// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/Building.h"

#include "AbilitySystemComponent.h"

#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/Attributes/UnitBaseAttributes.h"
#include "Gameplay/Combat/CombatComponent.h"

ABuilding::ABuilding()
{
    // Set up defaults
    Team = EUnitTeam::Monster;

    // GAS
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySubsystem"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full); // I think full, because we want to see things everywhere?

    CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

bool ABuilding::RemoveResident(const ACreature* Resident)
{
	return Residents.RemoveAll([Resident](ACreature* const InResident) { return Resident == InResident; }) > 0;
}

void ABuilding::RemoveAllResidents()
{
	Residents.Empty();
}

bool ABuilding::AddResident(ACreature* Resident)
{
	// TODO: Verify there is space
	Residents.Add(Resident);
	return true;
}

void ABuilding::RemoveAllVisitors()
{
	Visitors.Empty();
}

void ABuilding::AddVisitor(ACreature* Visitor)
{
	Visitors.Add(Visitor);
}

void ABuilding::RemoveVisitor(const ACreature* Visitor)
{
	Visitors.RemoveAll([Visitor](ACreature* const InVisitor) { return Visitor == InVisitor; });
}

FVector ABuilding::GetBuildingEntrance() const
{
	return GetActorLocation() + BuildingEntranceOffset;
}

void ABuilding::BeginPlay()
{
    Super::BeginPlay();

    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }

    SetupBaseAttributes();

    CombatComponent->OnDeath.AddDynamic(this, &ABuilding::HandleDeath);
}

void ABuilding::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    if (IsValid(CombatComponent))
    {
        CombatComponent->OnDeath.RemoveAll(this);
    }
}

void ABuilding::SetupBaseAttributes()
{
    if (IsValid(BuildingAttributeValues))
    {
        FString Context = TEXT("DefaultUnitAttributeIter");
        BuildingAttributeValues->ForeachRow<FUnitBaseAttributes>(Context, [this](const FName& Key, const FUnitBaseAttributes& Value)
            {
                if (!AbilitySystemComponent->HasAttributeSetForAttribute(Value.Attribute))
                {
                    UE_LOG(LogTemp, Error, TEXT("Unit [%s]'s base attribute specifies a value for [%s]%s, but unit does not have that attribute"),
                        *GetDebugName(this),
                        *Key.ToString(),
                        *Value.Attribute.AttributeName
                    );
                    return;
                }

                double AttributeValue = Value.BaseValue;
                if (Value.Variation > 0)
                {
                    const int Variation = FMath::FloorToInt(Value.Variation);
                    AttributeValue += FMath::RandRange(-Variation, Variation);
                }

                AbilitySystemComponent->SetNumericAttributeBase(Value.Attribute, AttributeValue);

            });
    }
}

ABuildingController* ABuilding::GetBuildingController() const
{
    return Cast<ABuildingController>(GetController());
}

void ABuilding::HandleDeath()
{
    // TODO: Spawn break effects

    // TODO: Eject and vacate!
    for (auto& Visitor : Visitors)
    {

    }

    for (auto& Resident : Residents)
    {

    }

    this->Destroy();
}
