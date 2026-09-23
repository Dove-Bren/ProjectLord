// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ToastNotification.generated.h"

class UTexture2D;
class UVMToast;
class USelectionComponent;

UENUM(BlueprintType)
enum class EToastNotificationType : uint8
{
	ResearchComplete, // Research completed:
	ConstructionComplete, // Construction complete:
	UpgradeComplete, // Building upgraded:
	HeroDead, // Hero is dead
	BuildingDestroyed, // Sire, we lost

	NewQuest, // New quest:
	QuestComplete, // Quest completed:
};

USTRUCT(BlueprintType)
struct FToastNotification
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Toast")
	EToastNotificationType Type;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Toast")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Toast")
	TObjectPtr<USelectionComponent> Selection;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Toast")
	FText Content;

	FToastNotification() : FToastNotification(EToastNotificationType::NewQuest, nullptr, FText::GetEmpty()) {}
	FToastNotification(EToastNotificationType InType, UTexture2D* InIcon, FText InContent)
		: FToastNotification(InType, InIcon, InContent, nullptr) {}
	FToastNotification(EToastNotificationType InType, UTexture2D* InIcon, FText InContent, USelectionComponent* InSelection)
		: Type(InType), Icon(InIcon), Selection(InSelection), Content(InContent) {
	}

	UVMToast* MakeViewModel(UObject* Outer);
};
