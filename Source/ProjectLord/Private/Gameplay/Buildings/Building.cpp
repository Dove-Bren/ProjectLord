// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/Building.h"

#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"

#include "LordLogging.h"
#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/AI/BuildingController.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Units/Unit.h"
#include "UI/ViewModels/SelectionViewModel.h"
#include "UI/ViewModels/Generic/CombatDataViewModel.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"

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

void ABuilding::SetTeam(EGameTeam InTeam)
{
    Team = InTeam;
}

void ABuilding::SetBuildingGold(int InGold)
{
    BuildingGold = InGold;
    GoldVM->SetGold(BuildingGold);
}

void ABuilding::PlaceExitingUnit(AUnit* Unit)
{
    Unit->SetActorLocation(GetBuildingEntrance());
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

    if (bIndestructible)
    {
        CombatComponent->SetInvulnerable(true);
    }

    CombatComponent->OnDeath.AddDynamic(this, &ABuilding::HandleDeath);

    if (BuildingMesh)
    {
        BuildingMesh->SetStaticMesh(GetBuildingMesh());
    }

    // Set up selection Data
    SetupSelectionData(SelectionComponent);

    // Connect to game time
    if (auto State = GetWorld()->GetGameState<ALordGameState>())
    {
        State->OnGameDayChange.AddDynamic(this, &ABuilding::HandleGameDayChanged);
    }
}

void ABuilding::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    if (IsValid(CombatComponent))
    {
        CombatComponent->OnDeath.RemoveAll(this);
    }

    if (auto State = GetWorld()->GetGameState<ALordGameState>())
    {
        State->OnGameDayChange.RemoveAll(this);
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

void ABuilding::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
    }
}

bool ABuilding::HasMatchingGameplayTag(FGameplayTag Tag) const
{
    if (ensure(AbilitySystemComponent))
    {
        return AbilitySystemComponent->HasMatchingGameplayTag(Tag);
    }
    return false;
}

bool ABuilding::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
    if (ensure(AbilitySystemComponent))
    {
        return AbilitySystemComponent->HasAllMatchingGameplayTags(TagContainer);
    }
    return false;
}

bool ABuilding::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
    if (ensure(AbilitySystemComponent))
    {
        return AbilitySystemComponent->HasAnyMatchingGameplayTags(TagContainer);
    }
    return false;
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

void ABuilding::SetupSelectionData(USelectionComponent* InSelectionComponent)
{
    // All of this never changes
    InSelectionComponent->SetTeam(GetTeam());
    InSelectionComponent->SetName(GetBuildingName());
    InSelectionComponent->SetDesc(GetBuildingType()->BuildingDescription);
    InSelectionComponent->SetIcon(GetBuildingType()->BuildingIcon);

    InSelectionComponent->SetCombatDataVM(UVMCombatData::Make(this, CombatComponent));

    GoldVM = CreateLordVM<UVMGold>(this);
    GoldVM->SetGold(BuildingGold);
    GoldVM->SetGoldGeneration(GoldGeneratedPerDay);
    InSelectionComponent->SetGoldVM(GoldVM);
}

ABuildingController* ABuilding::GetBuildingController() const
{
    return Cast<ABuildingController>(GetController());
}

FText ABuilding::GetBuildingName() const
{
    if (!BuildingCustomName.IsEmpty())
    {
        return BuildingCustomName;
    }

    auto Def = GetBuildingType();
    return Def->BuildingName;
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

    this->Destroy();
}

void ABuilding::HandleGameDayChanged(int GameDay)
{
    if (GameDay > 0)
    {
        if (GoldGeneratedPerDay > 0)
        {
            SetBuildingGold(GetBuildingGold() + GoldGeneratedPerDay);
        }
    }
}
