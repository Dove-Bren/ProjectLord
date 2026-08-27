// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/Unit.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "LordLogging.h"
#include "UI/Units/HealthBarBase.h"
#include "UI/ViewModels/Units/UnitViewModel.h"
#include "UI/WidgetBlueprintClassRegistry.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/SelectionComponent.h"
#include "Gameplay/AI/UnitController.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "UI/ViewModels/Generic/CombatDataViewModel.h"
#include "UI/ViewModels/Generic/CreatureCategoryViewModel.h"

AUnit::AUnit() : ACharacter()
{
    // Set up defaults
    Team = EGameTeam::Monster;
    Category = ECreatureCategory::Standard;

    // Adjust character stuff
    auto Collision = GetCapsuleComponent();
    Collision->InitCapsuleSize(22.0f, 50.0f);
    Collision->SetCollisionProfileName(TEXT("Unit"), true); // "Unit" profile defined in code

    auto Movement = GetCharacterMovement();
    Movement->bUseControllerDesiredRotation = true;
    Movement->bUseRVOAvoidance = true;
    Movement->AvoidanceConsiderationRadius = 100.f;

    GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

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

void AUnit::FaceActor(AActor* OtherActor)
{
    // Face target
    auto Diff = OtherActor->GetActorLocation() - GetActorLocation();
    Diff.Z = 0;
    SetActorRotation(Diff.Rotation());
}

void AUnit::HandleUnitRecruited()
{
    // Register with team
    if (auto TeamState = GetTeamState())
    {
        TeamState->AddUnit(this);
    }
}

void AUnit::BeginPlay()
{
    Super::BeginPlay();

    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }

    // TODO: We can make it so this only shows up on mouseover (Issue #25)
    AddHealthbarWidget();
    
    RegisterAttributes();
    SetupBaseAttributes();

    // Set health and mana equal to their max
    CombatAttributeSet->ResetHealthMana();

    CombatComponent->OnDeath.AddDynamic(this, &AUnit::HandleDeath);
    CombatComponent->OnAttack.AddDynamic(this, &AUnit::HandleAttack);

    
    SetupSelectionData(SelectionComponent);
}

void AUnit::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    if (IsValid(CombatComponent))
    {
        CombatComponent->OnDeath.RemoveAll(this);
    }
}

void AUnit::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
    }
}

bool AUnit::HasMatchingGameplayTag(FGameplayTag Tag) const
{
    if (ensure(AbilitySystemComponent))
    {
        return AbilitySystemComponent->HasMatchingGameplayTag(Tag);
    }
    return false;
}

bool AUnit::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
    if (ensure(AbilitySystemComponent))
    {
        return AbilitySystemComponent->HasAllMatchingGameplayTags(TagContainer);
    }
    return false;
}

bool AUnit::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
    if (ensure(AbilitySystemComponent))
    {
        return AbilitySystemComponent->HasAnyMatchingGameplayTags(TagContainer);
    }
    return false;
}

AUnitController* AUnit::GetUnitController() const
{
    return Cast<AUnitController>(GetController());
}

void AUnit::SetTeam(EGameTeam InTeam)
{
    if (InTeam != Team)
    {
        // Remove from existing team before changing
        if (auto TeamState = GetTeamState())
        {
            TeamState->RemoveUnit(this);
        }

        Team = InTeam;
        SelectionComponent->SetTeam(InTeam);

        // Add to new team
        if (auto TeamState = GetTeamState())
        {
            TeamState->AddUnit(this);
        }
    }
}

bool AUnit::IsDead() const
{
    return CombatComponent->IsDead();
}

void AUnit::OnDeath_Implementation()
{
    PlayDeathAnimation();
    OnUnitDeath.Broadcast(this);
    OnFinalDeath();
}

void AUnit::OnFinalDeath()
{
    BP_OnFinalDeath();
    OnUnitFinalDeath.Broadcast(this);
    this->Destroy();
}

void AUnit::PlayDeathAnimation()
{
    if (IsValid(DeathAnimation))
    {
        PlayAnimMontage(DeathAnimation);
    }
}

void AUnit::RegisterAttributes()
{
    ;
}

void AUnit::SetupBaseAttributes()
{
    if (IsValid(ClassAttributeDefaults))
    {
        FString Context = TEXT("DefaultUnitAttributeIter");
        ClassAttributeDefaults->ForeachRow<FAttributeBaseValue>(Context, [this](const FName& Key, const FAttributeBaseValue& Value)
            {
                if (!AbilitySystemComponent->HasAttributeSetForAttribute(Value.Attribute))
                {
                    UE_LOG(LordUnit, Error, TEXT("Unit [%s]'s base attribute specifies a value for [%s]%s, but unit does not have that attribute"),
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

    if (IsValid(ClassAttributeGrowth))
    {
        UGameplayEffect* GE_LevelMod = NewObject<UGameplayEffect>(this, TEXT("LevelMod"));
        GE_LevelMod->DurationPolicy = EGameplayEffectDurationType::Infinite;

        auto LevelAttribute = GetCombatAttributeSet()->GetLevelAttribute();
        FString Context = TEXT("ClassAttributeGrowth");
        ClassAttributeGrowth->ForeachRow<FAttributeBaseValue>(Context, [this, LevelAttribute, GE_LevelMod](const FName& Key, const FAttributeBaseValue& Value)
            {
                if (!AbilitySystemComponent->HasAttributeSetForAttribute(Value.Attribute))
                {
                    UE_LOG(LordUnit, Error, TEXT("Unit [%s]'s growth attribute specifies a value for [%s]%s, but unit does not have that attribute"),
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

                FGameplayModifierInfo Mod;
                Mod.Attribute = Value.Attribute;
                Mod.ModifierOp = EGameplayModOp::AddFinal;
                FAttributeBasedFloat Curve;
                Curve.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(LevelAttribute, EGameplayEffectAttributeCaptureSource::Source, false);
                Curve.Coefficient = FMath::Floor(AttributeValue);
                Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Curve);
                GE_LevelMod->Modifiers.Add(MoveTemp(Mod));

            });

        FGameplayEffectSpec Spec(GE_LevelMod, AbilitySystemComponent->MakeEffectContext(), 1);
        LevelDamageModHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(Spec);
    }
}

void AUnit::SetupSelectionData(USelectionComponent* InSelectionComponent)
{
    // All of this never changes
    InSelectionComponent->SetTeam(GetTeam());
    InSelectionComponent->SetName(GetUnitName());
    InSelectionComponent->SetDesc(GetUnitType()->UnitDescription);
    InSelectionComponent->SetIcon(GetUnitType()->UnitIcon);

    InSelectionComponent->SetCombatDataVM(UVMCombatData::Make(this, CombatComponent));
    InSelectionComponent->SetCategoryVM(UVMCreatureCategory::Make(this, GetCategory()));
}

void AUnit::HandleDeath()
{
    GetUnitController()->NotifyUnitDied();

    // Rebroadcast
    OnDeath();
}

void AUnit::HandleAttack(AActor* Target, UCombatComponent* TargetComponent)
{
    FaceActor(Target);
}

void AUnit::InitUnitVM()
{
    if (IsValid(UnitVM))
    {
        return;
    }

    UnitVM = UVMUnit::CreateForUnit(this);
    UnitVM->InitializeAttributeListeners(AbilitySystemComponent, CombatAttributeSet);

    // Note: For now, team is only ever set on construction of the AUnit.
    UnitVM->SetTeam(Team);
}

void AUnit::AddHealthbarWidget()
{
    if (IsValid(HealthbarWidgetComponent))
    {
        return;
    }

    const UWidgetBlueprintClassRegistry* WidgetBlueprints = UWidgetBlueprintClassRegistry::Get();
    const TSubclassOf<UHealthBarBase> HealthBarWidgetClass = WidgetBlueprints ? WidgetBlueprints->UnitMiniHealthBarWidget.LoadSynchronous() : nullptr;
    if (!ensure(WidgetBlueprints) || !ensure(HealthBarWidgetClass))
    {
        return;
    }

    // If we're adding a healthbar, we're gonna need to make sure the VM is good to go.
    InitUnitVM();

    HealthbarWidgetComponent = Cast<UWidgetComponent>(
        AddComponentByClass(UWidgetComponent::StaticClass(), true, FTransform::Identity, true));

    HealthbarWidgetComponent->AttachToComponent(GetCapsuleComponent(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    HealthbarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    HealthbarWidgetComponent->SetDrawAtDesiredSize(true);
    HealthbarWidgetComponent->SetWidgetClass(HealthBarWidgetClass);

    FinishAddComponent(HealthbarWidgetComponent, true, FTransform{FVector{0.f, 0.f, -100.f}});
    HealthbarWidgetComponent->Activate();

    if (UHealthBarBase* Widget = Cast<UHealthBarBase>(HealthbarWidgetComponent->GetWidget()))
    {
        Widget->ReceiveUnitVM(UnitVM);
    }
}

void AUnit::ApplyLevelDamageMod(int Level)
{
    AbilitySystemComponent->SetActiveGameplayEffectLevel(LevelDamageModHandle, Level);
}

AGameTeamState* AUnit::GetTeamState() const
{
    if (auto GameState = GetWorld()->GetGameState<ALordGameState>())
    {
        return GameState->GetTeam(GetTeam());
    }
    return nullptr;
}

UAnimMontage* AUnit::GetAnimForAbilityType(EAbilityAnimType AbilityType) const
{
    // Try to get that exact anim type, and fall back to the basic montage
    if (AbilityAnimations.Contains(AbilityType))
    {
        return AbilityAnimations[AbilityType];
    }

    return AbilityAnimations[EAbilityAnimType::HarmingNormal];
}
