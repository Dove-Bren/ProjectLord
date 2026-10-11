// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/Creature.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include "Gameplay/Map/FogOfWarComponent.h"
#include "Gameplay/Map/FogOfWarSubsystem.h"
#include "Gameplay/ActorFadeComponent.h"
#include "Gameplay/GameplayUtils.h"
#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Map/MinimapComponent.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Combat/CombatComponent.h"
#include "Gameplay/Combat/GameplayEffect/VisitingBuildingGameplayEffect.h"
#include "Gameplay/Attributes/CreatureAttributeSet.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Buildings/ResidentialBuilding.h"
#include "Gameplay/Units/HeroBase.h"
#include "Gameplay/Interaction/SelectionComponent.h"
#include "UI/ViewModels/Generic/ActionViewModel.h"
#include "UI/ViewModels/Generic/CreatureCategoryViewModel.h"

#define MOVEMENT_STAT_TO_UE_SPEED(InMovement) (InMovement * 50)

ACreature::ACreature()
{
    Category = ECreatureCategory::Standard;

	CreatureAttributeSet = CreateDefaultSubobject<UCreatureAttributeSet>(TEXT("CreatureAttributeSet"));
    GraveComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grave"));

    GraveComponent->SetupAttachment(GetMesh()->GetAttachParent());
    GraveComponent->SetVisibility(false, true);
    GraveComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

    FogOfWarComponent = CreateDefaultSubobject<UFogOfWarComponent>(TEXT("Fog of War"));
    MinimapComponent = CreateDefaultSubobject<UMinimapComponent>(TEXT("Minimap"));
    FadeComponent = CreateDefaultSubobject<UActorFadeComponent>(TEXT("Fade"));
}

void ACreature::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, HomeBuilding);
    DOREPLIFETIME(ThisClass, CurrentVisitingBuilding);
    DOREPLIFETIME(ThisClass, DeadTime);
    DOREPLIFETIME(ThisClass, Action);
}

void ACreature::Sided_RegisterAttributes()
{
    Super::Sided_RegisterAttributes();

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

void ACreature::Client_SetupSelectionData(USelectionComponent* InSelectionComponent)
{
    Super::Client_SetupSelectionData(InSelectionComponent);

    UVMAction* VM = CreateLordVM<UVMAction>(this);
    OnCreatureActionChanged.AddWeakLambda(this, [VM](ECreatureAction Action)
    {
            VM->SetAction(Action);
    });
    VM->SetAction(GetAction());
    
    InSelectionComponent->SetActionVM(VM);

    InSelectionComponent->SetCategoryVM(UVMCreatureCategory::Make(this, GetCategory()));
}

void ACreature::BeginPlay()
{
    Super::BeginPlay();

    const float HalfHeight = GetSimpleCollisionHalfHeight();
    GraveComponent->SetRelativeLocation(FVector(0, 0, -HalfHeight));

    FogOfWarComponent->SetTeam(GetTeam());
    MinimapComponent->SetTeam(GetTeam());
    FadeComponent->FadeComplete.AddUObject(this, &ThisClass::OnFadeComplete);
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

void ACreature::ServerTick(float DeltaTime)
{
    Super::ServerTick(DeltaTime);

    if (IsDead())
    {
        DeadTime += DeltaTime;
        SetAction(ECreatureAction::Dead);
    }
}

void ACreature::ClientTick(float DeltaTime)
{
    Super::ClientTick(DeltaTime);

    // TODO: This doesn't need to happen every frame
    auto FogSubsystem = GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
    if (FogSubsystem)
    {
        if (auto PC = Cast<ALordPlayerController>(GetGameInstance()->GetFirstLocalPlayerController()))
        {
            if (PC->PlayerStateReady.IsComplete() && PC->TeamStateReady.IsComplete())
            {
                SetActorHiddenInGame(FogSubsystem->IsInFog(PC->GetTeam(), GetActorLocation()));
            }
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
        OnRep_Action();
    }
}

bool ACreature::ShouldFlee() const
{
    if (!HasBuilding())
    {
        return false; // Nowhere to flee to
    }

    return CombatComponent->GetHealth() < CombatComponent->GetMaxHealth() / 4;
}

void ACreature::OnRep_Team(EGameTeam OldTeam)
{
    Super::OnRep_Team(OldTeam);

    FogOfWarComponent->SetTeam(Team);
    MinimapComponent->SetTeam(Team);
}

void ACreature::OnRep_Action()
{
    OnCreatureActionChanged.Broadcast(Action);
}

void ACreature::OnRep_DeadTime()
{

}

void ACreature::NotifyResidenceAttacked(AResidentialBuilding* Building, AActor* AttackingActor, UCombatComponent* AttackingCombatComponent)
{
    if (bEvacuatesOnAttack && IsInsideBuilding() && GetVisitingBuilding() == Building)
    {
        // Leave the building if it's attacked, if we're done healing
        if (ShouldEvacuate())
        {
            LeaveCurrentBuilding();
        }
    }

    BP_OnBuildingAttacked(Building, AttackingActor, AttackingCombatComponent);

    if (!IsInsideBuilding())
    {
        // Set attacker as a revenge target, which will get picked up to attack if we don't have
        // a target already
        CombatComponent->AddExtraRevengeTarget(AttackingCombatComponent);
    }
}

bool ACreature::ShouldEvacuate() const
{
    return CombatComponent->GetHealth() >= CombatComponent->GetMaxHealth();
}

void ACreature::OnEnterBuilding(AResidentialBuilding* Building)
{
    FadeComponent->FadeOut(GetMesh(), 1.0f);
    //// TODO I think units actually fade when entering a building.
    //GetMesh()->SetVisibility(false, true);
    GetCharacterMovement()->SetAvoidanceEnabled(false);
    SetActorLocation(Building->GetBuildingEntrance());
    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(UGEVisitingBuilding::StaticClass(), 1, AbilitySystemComponent->MakeEffectContext());
    if (SpecHandle.IsValid())
    {
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
    }

    BP_OnEnterBuilding(Building);
}

void ACreature::OnExitBuilding(AResidentialBuilding* Building)
{
    AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(UGEVisitingBuilding::StaticClass(), AbilitySystemComponent);
    GetCharacterMovement()->SetAvoidanceEnabled(true);
    FadeComponent->FadeIn(GetMesh(), 1.0f);
    //GetMesh()->SetVisibility(true, true);
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

    // Make sure to add any recent combat participants
    auto RecentAttackers = CombatComponent->GetRecentAttackers();
    for (auto Attacker : RecentAttackers)
    {
        auto Actor = Attacker->GetOwner();
        if (AHeroBase* Hero = Cast<AHeroBase>(Actor))
        {
            if (Hero->IsAlive() && Hero->GetTeam() != GetTeam())
            {
                Heroes.AddUnique(Hero);
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

void ACreature::OnFadeComplete()
{
    // Note: We fade for multiple things including entering/
    // leaving buildings as well as death
    if (EDeathFadeState::Idle == FadeState)
    {
        return;
    }

    if (EDeathFadeState::FadeBodyOut == FadeState)
    {
        // Body finished fading out
        OnFinishFadeOutBody();

        if (!ShouldHaveGravestone())
        {
            OnFinalDeath();
            return;
        }

        GetMesh()->SetVisibility(false, true);
        //GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

        FadeState = EDeathFadeState::FadeGraveIn;
        GraveComponent->SetVisibility(true, true);
        FadeComponent->FadeIn(GraveComponent, 2.0f);
        OnBeginFadeInGrave();

    }
    else if (EDeathFadeState::FadeGraveIn == FadeState)
    {
        // Grave faded in. Start timer to do final death cleanup
        OnFinishFadeInGrave();
        GetWorld()->GetTimerManager().SetTimer(GraveTimer, FTimerDelegate::CreateWeakLambda(this, [this]() {
            // Fade out grave
            FadeComponent->FadeOut(GraveComponent, 2.0f);
            OnBeginFadeOutGrave();
        }), 60.0f, false);
    } else if (EDeathFadeState::FadeGraveOut == FadeState)
    {
        // Final grave fadeout happened. Ready to perma-die
        OnFinishFadeOutGrave();
        OnFinalDeath();
    }
}

void ACreature::StartDeathFade()
{
    if (ensure(EDeathFadeState::Idle == FadeState))
    {
        FadeState = EDeathFadeState::FadeBodyOut;
        FadeComponent->FadeOut(GetMesh(), 2.0f);
        OnBeginFadeOutBody();
    }
}
