// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/ViewModels/LordViewModelBase.h"

#include "LordGameStateViewModel.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UVMLordGameState : public UVMLordBase
{
    GENERATED_BODY()

public:
    UVMLordGameState();

    void Setup(class ALordGameState* State);

    double GetGameTime() const { return GameTime; }
    void SetGameTime(double InGameTime) { UE_MVVM_SET_PROPERTY_VALUE(GameTime, InGameTime); UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetGameDays); }

    float GetGameSpeed() const { return GameSpeed; }
    void SetGameSpeed(double InGameSpeed) { UE_MVVM_SET_PROPERTY_VALUE(GameSpeed, InGameSpeed); }

    UFUNCTION(FieldNotify, BlueprintPure, Category = "GameState|Time")
    int GetGameDays() const { return FMath::FloorToInt(GetGameTime()); }

    /*UFUNCTION(FieldNotify, BlueprintPure, Category = "GameState|Time")
    bool IsGamePaused() const { return ??? }*/

    UFUNCTION(BlueprintCallable, Category = "GameState|Time")
    void ModifyGameSpeed(float InSpeed);

    UFUNCTION(BlueprintCallable, Category = "GameState|Time")
    void SetGamePaused(bool bPaused);


protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "GameState|Time")
    double GameTime;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "GameState|Time")
    float GameSpeed;

    UPROPERTY()
    TWeakObjectPtr<class ALordGameState> ParentState;

private:
    UFUNCTION()
    void HandleGameTimeChanged(double InGameDay);
    
    UFUNCTION()
    void HandleGameSpeedChanged(float InGameSpeed);
};
