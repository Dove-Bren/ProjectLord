// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/ViewModels/LordViewModelBase.h"
#include "Gameplay/SelectionAction.h"

#include "SelectionActionTreeViewModel.generated.h"

class UVMSelectionAction;

USTRUCT(BlueprintType)
struct PROJECTLORD_API FPage
{
    GENERATED_BODY()

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TArray<UVMSelectionAction*> Actions;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UVMSelectionActionTree : public UVMLordBase
{
    GENERATED_BODY()

public:
    UVMSelectionActionTree();

    const TArray<UVMSelectionAction*>& GetActions() const { return CurrentActions; }


    // Setup functions
    void Reset();
    void AddPage(FString PageName, TArray<UVMSelectionAction*> PageActions);

    // Operation functions
    bool GoToPage(const FString& PageName);
    bool GoBack();
    bool GoToRoot();
    bool HasPage(const FString& PageName) const { return !!FindPage(PageName); }
    const FString& GetCurrentPage() const;
    bool IsAtRoot() const { return GetCurrentPage() == RootPageName; }

    static const FString RootPageName;

    UFUNCTION(BlueprintCallable)
    void RefreshPage();


protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = GetActions, Category = "Selection|Action")
    TArray<UVMSelectionAction*> CurrentActions;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Selection|Action")
    TMap<FString, FPage> Pages;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Selection|Action")
    TArray<FString> PageHistory;

    FPage* FindPage(const FString& PageName);
    const FPage* FindPage(const FString& PageName) const;
    bool GoToPageInternal(const FString& PageName, bool bUpdateHistory = true);

    void UpdateActions(TArray<UVMSelectionAction*> NewActions);

};

UCLASS(Blueprintable)
class PROJECTLORD_API UTreePageAction : public USelectionAction
{
    GENERATED_BODY()

public:

    void SetPage(FString InPageName) { PageName = InPageName; }
    FString GetPage() const { return PageName; }

    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context) const override;
    virtual bool Perform_Implementation(const FSelectionActionContext& Context) override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Tree")
    FString PageName;
};

UCLASS(Blueprintable)
class PROJECTLORD_API UTreeBackAction : public USelectionAction
{
    GENERATED_BODY()

public:

    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context) const override;
    virtual bool Perform_Implementation(const FSelectionActionContext& Context) override;

};
