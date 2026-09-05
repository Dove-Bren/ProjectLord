// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/Creature.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"

#include "Gameplay/FogOfWarComponent.h"
#include "Gameplay/FogOfWarSubsystem.h"
#include "Gameplay/GameplayUtils.h"
#include "Gameplay/LordPlayerController.h"
#include "Gameplay/MinimapComponent.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Combat/GameplayEffect/VisitingBuildingGameplayEffect.h"
#include "Gameplay/Attributes/CreatureAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Buildings/ResidentialBuilding.h"
#include "Gameplay/Units/HeroBase.h"
#include "Gameplay/SelectionComponent.h"
#include "UI/ViewModels/Generic/ActionViewModel.h"

#define MOVEMENT_STAT_TO_UE_SPEED(InMovement) (InMovement * 50)

ACreature::ACreature()
{
	CreatureAttributeSet = CreateDefaultSubobject<UCreatureAttributeSet>(TEXT("CreatureAttributeSet"));
    GraveComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grave"));

    GraveComponent->SetupAttachment(GetMesh()->GetAttachParent());
    GraveComponent->SetVisibility(false, true);
    GraveComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

    FogOfWarComponent = CreateDefaultSubobject<UFogOfWarComponent>(TEXT("Fog of War"));
    MinimapComponent = CreateDefaultSubobject<UMinimapComponent>(TEXT("Minimap"));
}

void ACreature::RegisterAttributes()
{
    bool bIgnored;
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CreatureAttributeSet->GetMovementAttribute())
        .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
            {
                GetCharacterMovement()->MaxWalkSpeed = MOVEMENT_STAT_TO_UE_SPEED(ChangeData.NewValue);
            });
    GetCharacterMovement()->MaxWalkSpeed = AbilitySystemComponent->GetGameplayAttributeValue(CreatureAttributeSet->GetMovementAttribute(), bIgnored);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CombatAttributeSet->GetSightAttribute())
        .AddWeakLambda(this, [this](const FOnAttributeChangeData& ChangeData)
            {
                FogOfWarComponent->SetRevealRadius(ChangeData.NewValue);
            });
    FogOfWarComponent->SetRevealRadius(AbilitySystemComponent->GetGameplayAttributeValue(CombatAttributeSet->GetSightAttribute(), bIgnored));
}

void ACreature::SetupSelectionData(USelectionComponent* InSelectionComponent)
{
    Super::SetupSelectionData(InSelectionComponent);

    UVMAction* VM = CreateLordVM<UVMAction>(this);
    OnCreatureActionChanged.AddWeakLambda(this, [VM](ECreatureAction Action)
    {
            VM->SetAction(Action);
    });
    VM->SetAction(GetAction());
    
    InSelectionComponent->SetActionVM(VM);
}

void ACreature::BeginPlay()
{
    Super::BeginPlay();

    const float HalfHeight = GetSimpleCollisionHalfHeight();
    GraveComponent->SetRelativeLocation(FVector(0, 0, -HalfHeight));

    FogOfWarComponent->SetTeam(GetTeam());
    MinimapComponent->SetTeam(GetTeam());
}

void ACreature::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    if (ensure(AbilitySystemComponent))
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CreatureAttributeSet->GetMovementAttribute())
            .RemoveAll(this);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CombatAttributeSet->GetSightAttribute())
            .RemoveAll(this);
    }
}

void ACreature::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsDead())
    {
        DeadTime += DeltaTime;
        SetAction(ECreatureAction::Dead);
        FadeTick();
    }

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

void ACreature::SetHomeBuilding(AResidentialBuilding* Building)
{
    HomeBuilding = Building;
}

void ACreature::LeaveCurrentBuilding()
{
    auto Building = GetVisitingBuilding();
    if (ensure(Building))
    {
        CurrentVisitingBuilding = nullptr;
        Building->RemoveVisitor(this);
        OnExitBuilding(Building);
    }
}

void ACreature::EnterBuilding(AResidentialBuilding* Building)
{
    if (!ensure(!IsInsideBuilding()))
    {
        LeaveCurrentBuilding();
    }

    CurrentVisitingBuilding = Building;
    Building->AddVisitor(this);
    OnEnterBuilding(Building);
}

void ACreature::SetAction(ECreatureAction InAction)
{ 
    if (Action != InAction)
    {
        Action = InAction;
        OnCreatureActionChanged.Broadcast(Action);
    }
}

void ACreature::SetTeam(EGameTeam InTeam)
{
    Super::SetTeam(InTeam);

    FogOfWarComponent->SetTeam(InTeam);
    MinimapComponent->SetTeam(InTeam);
}

void ACreature::OnEnterBuilding(AResidentialBuilding* Building)
{
    // TODO I think units actually fade when entering a building.
    GetMesh()->SetVisibility(false, true);
    GetCharacterMovement()->SetAvoidanceEnabled(false);
    SetActorLocation(Building->GetBuildingEntrance());
    auto VisitEffect = NewObject<UGEVisitingBuilding>(this, TEXT("Visiting Effect"));
    AbilitySystemComponent->ApplyGameplayEffectToSelf(VisitEffect, 1, AbilitySystemComponent->MakeEffectContext());

    BP_OnEnterBuilding(Building);
}

void ACreature::OnExitBuilding(AResidentialBuilding* Building)
{
    AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(UGEVisitingBuilding::StaticClass(), AbilitySystemComponent);
    GetCharacterMovement()->SetAvoidanceEnabled(true);
    GetMesh()->SetVisibility(true, true);
    BP_OnExitBuilding(Building);
}

void ACreature::OnDeath_Implementation()
{
    // Super wants to be overwritten, not called
    //Super::OnDeath_Implementation();

    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->SetAvoidanceEnabled(false);

    MinimapComponent->UnregisterFromMinimap();

    PlayDeathAnimation();
    
    // Fade out, and maybe turn into a grave
    StartDeathFade();

    OnUnitDeath.Broadcast(this);
}

void ACreature::OnFinalDeath()
{
    if (HomeBuilding.IsValid())
    {
        HomeBuilding.Get()->RemoveResident(this);
    }
    OnUnitFinalDeath.Broadcast(this);
    this->Destroy();
}

bool ACreature::AwardGoldToNearbyHeroes(int Gold)
{
    constexpr float Radius = 300;
    auto NearbyActors = UGameplayUtils::GetActorsNear(GetWorld(), this, Radius);
    TArray<AHeroBase*> Heroes;
    for (auto Actor : NearbyActors)
    {
        if (AHeroBase* Hero = Cast<AHeroBase>(Actor))
        {
            if (Hero->IsAlive() && Hero->GetTeam() != GetTeam())
            {
                Heroes.Add(Hero);
            }
        }
    }

    if (Heroes.IsEmpty())
    {
        return false;
    }

    int GoldEach = FMath::Max(1, FMath::CeilToInt((float) Gold / (float)Heroes.Num()));
    for (auto Hero : Heroes)
    {
        Hero->AwardGold(GoldEach);
    }
    return true;
}

void ACreature::StartDeathFade()
{
    if (ensure(FadingBodyStartTime == 0))
    {
        CreateFadeMaterials();
        FadingBodyStartTime = DeadTime;
        bFading = true;
        OnBeginFadeOutBody();
    }
}

void ACreature::StartFadingInGrave()
{
    if (ensure(FadingGraveStartTime == 0))
    {
        GraveComponent->SetVisibility(true, true);
        FadingGraveStartTime = DeadTime;
        OnBeginFadeInGrave();
    }
}

void ACreature::StartFadingOutGrave()
{
    // Should not be called before fading in
    if (ensure(FadingGraveStartTime != 0))
    {
        FadingGraveStartTime = -DeadTime;
        OnBeginFadeOutGrave();
    }
}

void ACreature::FadeTick()
{
    constexpr float FADE_TIME = 2.0f;
    constexpr float GRAVE_TIME = 60.0f;

    // Fading out grave?
    if (FadingGraveStartTime < 0)
    {
        const float ActualTime = -FadingGraveStartTime;
        const float Diff = DeadTime - ActualTime;
        const float Percent = FMath::Clamp(Diff / FADE_TIME, 0.0f, 1.0f);
        OnUpdateFadeGrave(Percent);
        FadeGraveTo(1.0f - Percent);
        if (Percent >= 1.0f)
        {
            OnFinishFadeOutGrave();
            OnFinalDeath();
        }
    }
    // Fading in grave?
    else if (FadingGraveStartTime > 0)
    {
        if (!GraveTimer.IsValid())
        {
            const float Diff = DeadTime - FadingGraveStartTime;
            const float Percent = FMath::Clamp(Diff / FADE_TIME, 0.0f, 1.0f);
            OnUpdateFadeGrave(Percent);
            FadeGraveTo(Percent);
            if (Percent >= 1.0f)
            {
                OnFinishFadeInGrave();
                GetWorld()->GetTimerManager().SetTimer(GraveTimer, FTimerDelegate::CreateWeakLambda(this, [this]() {
                    StartFadingOutGrave();
                    }), GRAVE_TIME, false);
            }
        }
    }
    // Fading body?
    else if (bFading)
    {
        const float Diff = DeadTime - FadingBodyStartTime;
        const float Percent = FMath::Clamp(Diff / FADE_TIME, 0.0f, 1.0f);
        OnUpdateFadeOutBody(Percent);
        FadeBodyTo(1.0f - Percent);
        if (Percent >= 1.0f)
        {
            OnFinishFadeOutBody();

            if (!ShouldHaveGravestone())
            {
                OnFinalDeath();
                return;
            }

            GetMesh()->SetVisibility(false, true);
            //GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
            GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
            StartFadingInGrave();
        }
    }
}

void ACreature::CreateFadeMaterials()
{
    if (ensure(BodyFadeMaterials.IsEmpty()))
    {
        for (int i = 0; i < GetMesh()->GetNumMaterials(); i++)
        {
            BodyFadeMaterials.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(i));
        }

        for (int i = 0; i < GraveComponent->GetNumMaterials(); i++)
        {
            GraveFadeMaterials.Add(GraveComponent->CreateAndSetMaterialInstanceDynamic(i));
        }
    }
}

void ACreature::FadeBodyTo(float Percent)
{
    for (auto BodyMaterial : BodyFadeMaterials)
    {
        BodyMaterial->SetScalarParameterValue(TEXT("FadeAlpha"), Percent);
    }
}

void ACreature::FadeGraveTo(float Percent)
{
    for (auto GraveMaterial : GraveFadeMaterials)
    {
        GraveMaterial->SetScalarParameterValue(TEXT("FadeAlpha"), Percent);
    }
}
