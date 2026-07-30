// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/Building.h"

#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"

#include "LordLogging.h"
#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Combat/CombatComponent.h"

ABuilding::ABuilding()
{
    // Set up defaults
    Team = EGameTeam::Monster;
    MaxLevel = 1;
    BuildingLevel = 1; // TODO: Building, could set this to 0 to denote that it hasn't been built yet

    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
    BuildingMesh->SetMobility(EComponentMobility::Stationary);
    SetRootComponent(BuildingMesh);

    // GAS
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySubsystem"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full); // I think full, because we want to see things everywhere?

    CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));

    SelectionComponent = CreateDefaultSubobject<USelectionComponent>(TEXT("Selection"));
    SelectionComponent->SetSelectable(true);

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

UStaticMesh* ABuilding::GetBuildingMesh() const
{
    // TODO: Construction, and picking from BuildingConstructionMeshes if
    // being build.
    // if (BuildingLevel == 0) ...

    const int LevelToUse = FMath::Max(BuildingLevel, 1); // TODO remove when above happens
    // Note: should only allow to fail in preview mode?
    if (ensure(LevelToUse - 1 < BuildingLevelMeshes.Num()))
    {
        return BuildingLevelMeshes[LevelToUse - 1];
    }

    return nullptr;
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

    if (BuildingMesh)
    {
        BuildingMesh->SetStaticMesh(GetBuildingMesh());
    }
}

void ABuilding::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    if (IsValid(CombatComponent))
    {
        CombatComponent->OnDeath.RemoveAll(this);
    }
}

void ABuilding::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (BuildingMesh)
    {
        BuildingMesh->SetStaticMesh(GetBuildingMesh());
    }
}

void ABuilding::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // TODO: Building. Check if it's done?
}

void ABuilding::SetupBaseAttributes()
{
    if (IsValid(BuildingAttributeValues))
    {
        FString Context = TEXT("DefaultBuildingAttributeIter");
        BuildingAttributeValues->ForeachRow<FAttributeBaseValue>(Context, [this](const FName& Key, const FAttributeBaseValue& Value)
            {
                if (!AbilitySystemComponent->HasAttributeSetForAttribute(Value.Attribute))
                {
                    UE_LOG(LordBuilding, Error, TEXT("Building [%s]'s base attribute specifies a value for [%s]%s, but building does not have that attribute"),
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

int ABuilding::GetBuildingHealth() const
{
    bool bIgnored;
    return FMath::FloorToInt(AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetHealthAttribute(), bIgnored));
}

int ABuilding::GetBuildingMaxHealth() const
{
    bool bIgnored;
    return FMath::FloorToInt(AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetMaxHealthAttribute(), bIgnored));
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
