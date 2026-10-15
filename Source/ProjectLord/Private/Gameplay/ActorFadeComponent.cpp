// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/ActorFadeComponent.h"

#include "Curves/CurveFloat.h"

/*static*/ const FName UActorFadeComponent::OpacityMaterialPropName = TEXT("Opacity");

UActorFadeComponent::UActorFadeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = true;
}

void UActorFadeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UActorFadeComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EFadeOperation::Idle == FadeOperation)
	{
		return;
	}

	if (EFadeOperation::Idle != FadeOperation)
	{
		// Get progress
		float Prog = 0.f;
		if (EFadeOperation::FadeDynamic == FadeOperation)
		{
			if (ensure(FadeOpacityGetter))
			{
				Prog = FMath::Clamp(FadeOpacityGetter.GetValue()(), 0, 1);
			}
		}
		else if (EFadeOperation::FadeOut == FadeOperation
			|| EFadeOperation::FadeIn == FadeOperation)
		{
			const bool bFadeOut = EFadeOperation::FadeOut == FadeOperation;
			FadeProgress += DeltaTime;
			
			// When fading out, progress is simply prog/duration
			// When fading in, we reverse the number to walk the curve backwards
			// instead of flipping the curve value
			Prog = FMath::Clamp(FadeProgress / FadeDuration, 0, 1);
			if (!bFadeOut) Prog = 1 - Prog;

			// Clear operation if we're done, too. Then fall through to fade and deactivate
			if (FadeProgress >= FadeDuration)
			{
				FadeOperation = EFadeOperation::Idle;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Fade Operation not handled: %d"), FadeOperation);
		}

		// Translate to opacity and do fade
		const float Opacity = FadeCurve ? FadeCurve->GetFloatValue(Prog) : Prog;
		FadeTo(Opacity);
	}

	if (EFadeOperation::Idle == FadeOperation)
	{
		// Idle, so stop ticking and wait for another activation
		SetComponentTickEnabled(false);
		FadeComplete.Broadcast();
		return;
	}
}

void UActorFadeComponent::FadeOut(UPrimitiveComponent* Mesh, float InDuration)
{
	//ensure(EFadeOperation::Idle == FadeOperation); Was happening fairly often with peasants \
	// entering and leaving buildings frequently
	SetupFor(Mesh);
	FadeOperation = EFadeOperation::FadeIn;
	FadeDuration = InDuration;
	FadeProgress = 0;

	// Cleanup to make debugging easier
	FadeOpacityGetter = NullOpt;

	SetComponentTickEnabled(true);
}

void UActorFadeComponent::FadeIn(UPrimitiveComponent* Mesh, float InDuration)
{
	//ensure(EFadeOperation::Idle == FadeOperation); Was happening fairly often with peasants \
	// entering and leaving buildings frequently
	SetupFor(Mesh);
	FadeOperation = EFadeOperation::FadeOut;
	FadeDuration = InDuration;
	FadeProgress = 0;

	// Cleanup to make debugging easier
	FadeOpacityGetter = NullOpt;

	SetComponentTickEnabled(true);

	// Things are usually fully opaque by default, so hackily force to 0 opacity
	// here
	FadeTo(0);
}

void UActorFadeComponent::FadeDynamic(UPrimitiveComponent* Mesh, FGetDynamicOpacity InGetter)
{
	ensure(EFadeOperation::Idle == FadeOperation);
	SetupFor(Mesh);
	FadeOperation = EFadeOperation::FadeDynamic;
	FadeOpacityGetter = InGetter;

	// Cleanup to make debugging easier
	FadeDuration = -1;
	FadeProgress = -1;

	SetComponentTickEnabled(true);
}

void UActorFadeComponent::EndFade(float FinalProg)
{
	if (EFadeOperation::Idle != FadeOperation)
	{
		if (FinalProg != -1)
		{
			const float Opacity = FadeCurve ? FadeCurve->GetFloatValue(FMath::Clamp(FinalProg, 0, 1)) : FMath::Clamp(FinalProg, 0, 1);
			FadeTo(Opacity);
		}

		FadeOperation = EFadeOperation::Idle;
		SetComponentTickEnabled(false);

		FadeComplete.Broadcast();
	}
}

void UActorFadeComponent::FadeTo(float Opacity)
{
	for (auto Material : FadeMaterials)
	{
		Material->SetScalarParameterValue(OpacityMaterialPropName, Opacity);
	}
}

void UActorFadeComponent::SetupFor(UPrimitiveComponent* Mesh)
{
	FadeMaterials.Empty();

	if (CachedMaterials.Contains(Mesh))
	{
		FadeMaterials = CachedMaterials[Mesh].Materials;
	}

	for (int i = 0; i < Mesh->GetNumMaterials(); i++)
	{
		FadeMaterials.Add(Mesh->CreateAndSetMaterialInstanceDynamic(i));
	}

	FMaterialCacheRow Row = {};
	Row.Materials = FadeMaterials; // Copy

	CachedMaterials.Add(Mesh, MoveTemp(Row));
}

//void UActorFadeComponent::UpdateMesh(float ConstructionProgress)
//{
//	static constexpr float Values[] = {.4f, .6f, .8f, 1.0f};
//	// find what value to use
//	float Value = Values[0];
//	for (float V : Values) {
//		if (V > ConstructionProgress) break;
//		Value = V;
//	}
//
//	SetupMaterials();
//	for (auto Material : BuildingFadeMaterials)
//	{
//		Material->SetScalarParameterValue(TEXT("ConstructionProgress"), Value);
//	}
//}
