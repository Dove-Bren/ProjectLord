// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "Gameplay/GameTeam.h"
#include "MinimapIconViewModel.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMMinimapIcon : public UVMLordBase
{
    GENERATED_BODY()

public:

    EGameTeam GetTeam() const { return Team; }
    void SetTeam(EGameTeam InTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, InTeam); }

    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

    FVector GetWorldLocation() const { return WorldLocation; }
    void SetWorldLocation(FVector InWorldLocation) { UE_MVVM_SET_PROPERTY_VALUE(WorldLocation, InWorldLocation); }

    bool IsVisible() const { return bVisible; }
    void SetVisible(bool InVisible) { UE_MVVM_SET_PROPERTY_VALUE(bVisible, InVisible); }


protected:
    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "MapIcon")
    EGameTeam Team;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "MapIcon")
    UTexture2D* Icon;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "MapIcon")
    FVector WorldLocation;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = IsVisible, Category = "MapIcon")
    bool bVisible;
};
