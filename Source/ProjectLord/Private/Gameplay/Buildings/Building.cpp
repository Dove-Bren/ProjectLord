// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/Building.h"

#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NavModifierComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Engine/OverlapResult.h"

#include "LordLogging.h"
#include "Gameplay/FogOfWarComponent.h"
#include "Gameplay/FogOfWarSubsystem.h"
#include "Gameplay/GameTeam.h"
#include "Gameplay/LordPlayerController.h"
#include "Gameplay/MinimapComponent.h"
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
#include "UI/InspectWidget.h"
#include "UI/ToastNotification.h"
#include "UI/WidgetBlueprintClassRegistry.h"
#include "UI/ViewModels/SelectionViewModel.h"
#include "UI/ViewModels/SelectionActionTreeViewModel.h"
#include "UI/ViewModels/Buildings/BuildingViewModel.h"
#include "UI/ViewModels/Generic/CombatDataViewModel.h"
#include "UI/ViewModels/Generic/GoldViewModel.h"
#include "UI/ViewModels/Generic/InspectableViewModel.h"
#include "UI/ViewModels/Generic/LevelViewModel.h"

ABuilding::ABuilding()
{
    // Set up defaults
    Team = EGameTeam::Neutral;
    MaxLevel = 1;
    BuildingLevel = 1; // This is for map buildings that are already built. Set back to 0 in placement logic.
    BuildingAvailableLevel = 1;
    bToastWhenConstructed = false;
    bToastWhenDestroyed = false;

    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
    BuildingMesh->SetMobility(EComponentMobility::Stationary);
    BuildingMesh->SetCollisionObjectType(ECC_Pawn);
    BuildingMesh->SetCanEverAffectNavigation(false);
    RootComponent = BuildingMesh;

    Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    Collision->SetupAttachment(RootComponent);

    NavMeshMod = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavMeshMod"));
    NavMeshMod->SetAreaClass(UNavArea_Obstacle::StaticClass());

    // GAS
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySubsystem"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full); // I think full, because we want to see things everywhere?

    BuildingAttributeSet = CreateDefaultSubobject<UBuildingAttributeSet>(TEXT("BuildingAttributeSet"));

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));

    SelectionComponent = CreateDefaultSubobject<USelectionComponent>(TEXT("Selection"));
    SelectionComponent->SetSelectable(true);

    FadeComponent = CreateDefaultSubobject<UBuildingConstructionFadeComponent>(TEXT("ConstructionFade"));

    FogOfWarComponent = CreateDefaultSubobject<UFogOfWarComponent>(TEXT("Fog of War"));

    MinimapComponent = CreateDefaultSubobject<UMinimapComponent>(TEXT("Minimap"));

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void ABuilding::SetTeam(EGameTeam InTeam)
{
    if (Team == InTeam)
    {
        return;
    }

    // Clean up old team
    if (auto State = GetWorld()->GetGameState<ALordGameState>())
    {
        if (AGameTeamState* TeamState = State->GetTeam(GetTeam()))
        {
            TeamState->RemoveBuilding(this);
        }
    }

    Team = InTeam;
    SelectionComponent->SetTeam(InTeam);
    FogOfWarComponent->SetTeam(InTeam);
    MinimapComponent->SetTeam(InTeam);

    // relookup team state now that it's changed and add ourselves
    if (auto State = GetWorld()->GetGameState<ALordGameState>())
    {
        if (AGameTeamState* TeamState = State->GetTeam(GetTeam()))
        {
            TeamState->AddBuilding(this);
        }
    }
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
        AbilitySystemComponent->SetNumericAttributeBase(BuildingAttributeSet->GetHealthAttribute(),
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

void ABuilding::AddBuildingGold(int InGold)
{
    SetBuildingGold(FMath::Max(GetBuildingGold() + InGold));
}

int ABuilding::CollectBuildingGold()
{
    const int Gold = GetBuildingGold();
    SetBuildingGold(0);

    // Notify of collection
    if (auto State = GetWorld()->GetGameState<ALordGameState>())
    {
        if (AGameTeamState* TeamState = State->GetTeam(GetTeam()))
        {
            TeamState->NotifyBuildingTaxCollected(this);
        }
    }

    return Gold;
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
            Collision->SetBoxExtent(Extent * FVector(2, 2, 1));
            Collision->SetRelativeLocation(FVector(0, 0, Extent.Z));
            BuildingEntranceOffset = FVector(0, (Extent.X + 50), 0).RotateAngleAxis(BuildingRotation.Yaw, FVector(0, 0, 1));
            NavMeshMod->UpdateNavigationBounds();
        }
    }
}

bool ABuilding::WantsTaxCollection() const
{
    return GetBuildingGold() > 0;
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

    // Notify team state
    // TODO maybe make event and make team state listen to all buildings?
    if (auto State = GetWorld()->GetGameState<ALordGameState>())
    {
        if (AGameTeamState* TeamState = State->GetTeam(GetTeam()))
        {
            TeamState->NotifyBuildingRepairComplete(this);
        }
    }

    OnRepairComplete();
}

void ABuilding::HandleBuildingUpgraded()
{
    // Update building level
    SetLevel(BuildingAvailableLevel);

    if (bToastWhenConstructed)
    if (auto State = GetWorld()->GetGameState<ALordGameState>())
    {
        if (AGameTeamState* TeamState = State->GetTeam(GetTeam()))
        {
            if (auto LordController = TeamState->GetPrimaryPlayerController())
            {
                const auto ToastType = BuildingAvailableLevel == 1 ? EToastNotificationType::ConstructionComplete
                                        : EToastNotificationType::UpgradeComplete;
                LordController->AddToastNotification(FToastNotification(ToastType, BuildingType->BuildingIcon, GetBuildingName(), SelectionComponent));
            }
        }
    }

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

    bool bIgnored;
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BuildingAttributeSet->GetSightAttribute())
        .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
            {
                FogOfWarComponent->SetRevealRadius(ChangeData.NewValue);
            });
    FogOfWarComponent->SetRevealRadius(AbilitySystemComponent->GetGameplayAttributeValue(BuildingAttributeSet->GetSightAttribute(), bIgnored));
    FogOfWarComponent->SetTeam(GetTeam());

    MinimapComponent->SetTeam(GetTeam());

    CombatComponent->OnDeath.AddDynamic(this, &ABuilding::HandleDeath);
    CombatComponent->OnHealthChange.AddDynamic(this, &ABuilding::HandleHealthChanged);

    RefreshMesh();

    // Before selection data, so we can reuse VMs
    SetupViewModel();

    // Set up selection Data
    SetupSelectionData(SelectionComponent);

    // Connect to game time
    if (auto State = GetWorld()->GetGameState<ALordGameState>())
    {
        State->OnGameDayChange.AddDynamic(this, &ABuilding::HandleGameDayChanged);
    }

    // Trigger a 'team change' to register with team
    {
        const EGameTeam RealTeam = GetTeam();
        Team = EGameTeam::Neutral == RealTeam ? EGameTeam::Monster : EGameTeam::Neutral;
        SetTeam(RealTeam);
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

    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BuildingAttributeSet->GetSightAttribute())
            .RemoveAll(this);
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

    // TODO: This doesn't need to happen every frame
    auto FogSubsystem = GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
    if (FogSubsystem)
    {
        if (auto PC = Cast<ALordPlayerController>(GetGameInstance()->GetFirstLocalPlayerController()))
        {
            SetActorHiddenInGame(FogSubsystem->IsInFog(PC->GetTeam(), GetActorLocation()));
        }
    }
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

    InSelectionComponent->SetCombatDataVM(CombatVM);

    InSelectionComponent->SetGoldVM(GoldVM);

    const UWidgetBlueprintClassRegistry* WidgetBlueprints = UWidgetBlueprintClassRegistry::Get();
    const TSubclassOf<UInspectWidget> BuildingInspectWidgetClass = WidgetBlueprints ? WidgetBlueprints->BuildingInspectWidget.LoadSynchronous() : nullptr;
    if (ensure(BuildingInspectWidgetClass))
    {
        auto InspectVM = UVMInspectable::Make(this, BuildingInspectWidgetClass, FGetInspectVM::CreateWeakLambda(this, [this]() {
            return BuildingVM;
        }));
        InSelectionComponent->SetInspectVM(InspectVM);
    }

    OnBuildingLevelChanged.AddWeakLambda(this, [this, InSelectionComponent](int NewLevel)
        {
            if (auto Tree = InSelectionComponent->GetActionTreeVM())
            {
                Tree->RefreshPage();
            }
        });
}

void ABuilding::SetupViewModel()
{
    CombatVM = UVMCombatData::Make(this, CombatComponent);

    GoldVM = CreateLordVM<UVMGold>(this);
    GoldVM->SetGold(BuildingGold);
    GoldVM->SetGoldGeneration(GetGoldGeneratedPerDay());
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BuildingAttributeSet->GetGoldGenerationAttribute())
        .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
            {
                GoldVM->SetGoldGeneration(ChangeData.NewValue);
            });

    LevelVM = CreateLordVM<UVMLevel>(this);
    LevelVM->SetLevel(GetBuildingLevel());

    OnBuildingLevelChanged.AddWeakLambda(this, [this](int NewLevel)
    {
        LevelVM->SetLevel(NewLevel);
    });

    BuildingVM = CreateLordVM<UVMBuilding>(this);

    auto Type = GetBuildingType();
    if (ensure(Type))
    {
        BuildingVM->SetName(Type->BuildingName);
        BuildingVM->SetDescription(Type->BuildingDescription);
        BuildingVM->SetIcon(Type->BuildingIcon);
    }
    BuildingVM->SetCustomName(BuildingCustomName);
    BuildingVM->SetTeam(GetTeam());

    BuildingVM->SetLevelVM(LevelVM);
    BuildingVM->SetGoldVM(GoldVM);
    BuildingVM->SetCombatVM(CombatVM);
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
    return FMath::FloorToInt(AbilitySystemComponent->GetGameplayAttributeValue(BuildingAttributeSet->GetHealthAttribute(), bIgnored));
}

int ABuilding::GetBuildingMaxHealth() const
{
    bool bIgnored;
    return FMath::FloorToInt(AbilitySystemComponent->GetGameplayAttributeValue(BuildingAttributeSet->GetMaxHealthAttribute(), bIgnored));
}

int ABuilding::GetGoldGeneratedPerDay() const
{
    bool bIgnored;
    return FMath::FloorToInt(AbilitySystemComponent->GetGameplayAttributeValue(BuildingAttributeSet->GetGoldGenerationAttribute(), bIgnored));
}

void ABuilding::HandleDeath()
{
    // TODO: Spawn break effects?

    if (bToastWhenDestroyed)
    if (auto State = GetWorld()->GetGameState<ALordGameState>())
    {
        if (AGameTeamState* TeamState = State->GetTeam(GetTeam()))
        {
            if (auto LordController = TeamState->GetPrimaryPlayerController())
            {
                LordController->AddToastNotification(FToastNotification(EToastNotificationType::BuildingDestroyed, BuildingType->BuildingIcon, GetBuildingName(), SelectionComponent));
            }
        }
    }

    OnBuildingDestroyed.Broadcast(this);

    this->Destroy();
}

void ABuilding::HandleGameDayChanged(int GameDay)
{
    if (GameDay > 0)
    {
        const int Generated = GetGoldGeneratedPerDay();
        if (Generated > 0)
        {
            SetBuildingGold(GetBuildingGold() + Generated);
        }
    }
}

void ABuilding::HandleHealthChanged(int Health, int MaxHealth)
{
    OnBuildingHealthChanged.Broadcast(Health, MaxHealth);
    if (!AtFullHealth())
    {
        OnBuildingNeedsRepairsChanged.Broadcast(true);
    }
}

void ABuilding::HandleBuildingPlacement_Implementation()
{
    SetLevel(0);
    SetAvailableLevel(1);
    AbilitySystemComponent->SetNumericAttributeBase(BuildingAttributeSet->GetHealthAttribute(),
        (int) ((float) GetBuildingMaxHealth() * 0.1f));

    FadeComponent->Activate();
    FadeComponent->Enable();

    ClearFoliageAround();
}

void ABuilding::ClearFoliageAround()
{
    // Do 3x the size of the actual building (one building's length on each side)
    auto Extent = BuildingMesh->GetBounds().BoxExtent;
    auto Shape = FCollisionShape::MakeBox(Extent * 3);

    TArray<FOverlapResult> Overlaps;
    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel2); // Foliage custom channel
    if (GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, ObjectParams, Shape))
    {
        for (auto& Overlap : Overlaps)
        {
            if (auto ISC = Cast<UInstancedStaticMeshComponent>(Overlap.GetComponent()))
            {
                ISC->RemoveInstance(Overlap.GetItemIndex());
            }
        }
    }
}
