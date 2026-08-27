// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/Building.h"

#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NavModifierComponent.h"
#include "Components/BoxComponent.h"
#include "NavAreas/NavArea_Obstacle.h"

#include "LordLogging.h"
#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/AI/BuildingController.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Buildings/BuildingConstructionFadeComponent.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Combat/GameplayEffect/GenericGameplayTagEffect.h"
#include "Gameplay/Units/Unit.h"
#include "UI/ViewModels/SelectionViewModel.h"
#include "UI/ViewModels/Generic/CombatDataViewModel.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"

ABuilding::ABuilding()
{
    // Set up defaults
    Team = EGameTeam::Monster;
    MaxLevel = 1;
    BuildingLevel = 1; // This is for map buildings that are already built. Set back to 0 in placement logic.
    BuildingAvailableLevel = 1;

    Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    SetRootComponent(Collision);

    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
    BuildingMesh->SetMobility(EComponentMobility::Stationary);
    BuildingMesh->SetupAttachment(Collision);

    NavMeshMod = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavMeshMod"));
    NavMeshMod->SetAreaClass(UNavArea_Obstacle::StaticClass());

    // GAS
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySubsystem"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full); // I think full, because we want to see things everywhere?

    CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));

    SelectionComponent = CreateDefaultSubobject<USelectionComponent>(TEXT("Selection"));
    SelectionComponent->SetSelectable(true);

    FadeComponent = CreateDefaultSubobject<UBuildingConstructionFadeComponent>(TEXT("ConstructionFade"));

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABuilding::SetTeam(EGameTeam InTeam)
{
    Team = InTeam;
    SelectionComponent->SetTeam(InTeam);
}

bool ABuilding::CanLevelUp() const
{
    return !IsMaxLevel()
        && !IsUnderConstruction()
        && GetBuildingHealth() >= GetBuildingMaxHealth();
}

void ABuilding::StartLevelUp()
{
    if (ensure(CanLevelUp()))
    {
        // We want to keep current health the same, so save it and restore it
        const int Health = GetBuildingHealth();
        SetAvailableLevel(GetBuildingLevel() + 1); // Updates health values

        // restore
        AbilitySystemComponent->SetNumericAttributeBase(CombatAttributeSet->GetHealthAttribute(),
            Health);
    }
}

void ABuilding::SetAvailableLevel(int InAvailableLevel)
{
    BuildingAvailableLevel = InAvailableLevel;
    SetupBaseAttributes();
    OnBuildingAvailableLevelChanged.Broadcast(GetBuildingAvailableLevel());
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

void ABuilding::RefreshMesh()
{
    if (BuildingMesh)
    {
        if (BuildingMesh->SetStaticMesh(GetBuildingMesh()))
        {
            auto Extent = BuildingMesh->GetBounds().BoxExtent;
            auto BuildingRotation = GetActorRotation();
            Collision->SetBoxExtent(Extent);
            BuildingEntranceOffset = FVector(0, (Extent.X + 50), 0).RotateAngleAxis(BuildingRotation.Yaw, FVector(0, 0, 1));
            NavMeshMod->UpdateNavigationBounds();
        }
    }
}

bool ABuilding::WantsRepair() const
{
    return GetBuildingHealth() < GetBuildingMaxHealth();
}

void ABuilding::NotifyRepairAction()
{
    RefreshMesh();

    OnRepairActionReceived();
}

void ABuilding::NotifyRepairComplete()
{
    RefreshMesh();

    // Newly constructed?
    if (BuildingLevel < BuildingAvailableLevel)
    {
        HandleBuildingUpgraded();
    }

    OnRepairComplete();
}

void ABuilding::HandleBuildingUpgraded()
{
    // Update building level
    SetLevel(BuildingAvailableLevel);

    OnUpgradeComplete();
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

        // Also make untargetable
        auto* UntargetEffect = NewObject<UGEGenericGameplayTag>(this, TEXT("Untargetable Effect"));
        UntargetEffect->AddTag(ULordGameplayTags::UnitStateUntargetable());
        AbilitySystemComponent->ApplyGameplayEffectToSelf(UntargetEffect, 1, AbilitySystemComponent->MakeEffectContext());
    }

    CombatComponent->OnDeath.AddDynamic(this, &ABuilding::HandleDeath);
    CombatComponent->OnHealthChange.AddDynamic(this, &ABuilding::HandleHealthChanged);

    RefreshMesh();

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

    RefreshMesh();
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
    // Note: using 'available' level here on purpose
    const int LevelIdx = FMath::Max(0, GetBuildingAvailableLevel() - 1);
    if (ensure(BuildingLevelAttributeValues.Num() >= LevelIdx))
    {
        FString Context = TEXT("DefaultBuildingAttributeIter");
        const auto& LevelMap = BuildingLevelAttributeValues[LevelIdx];
        LevelMap->ForeachRow<FAttributeBaseValue>(Context, [this](const FName& Key, const FAttributeBaseValue& Value)
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

void ABuilding::HandleHealthChanged(int Health, int MaxHealth)
{
    OnBuildingHealthChanged.Broadcast(Health, MaxHealth);
    if (WantsRepair())
    {
        OnBuildingNeedsRepairsChanged.Broadcast(true);
    }
}

void ABuilding::HandleBuildingPlacement_Implementation()
{
    SetLevel(0);
    SetAvailableLevel(1);
    AbilitySystemComponent->SetNumericAttributeBase(CombatAttributeSet->GetHealthAttribute(),
        (int) ((float) GetBuildingMaxHealth() * 0.1f));

    FadeComponent->Activate();
    FadeComponent->Enable();
}
