// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/ViewModels/LordViewModelBase.h"
#include "UI/ViewModels/SelectionActionViewModel.h"
#include "Gameplay/Interaction/SelectionAction.h"

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

    FString GetPage() const { return PageName; }

    virtual bool CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const override
    {
        return true;
    }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Selection|Action|Tree")
    FString PageName;

    virtual UVMSelectionAction* ConstructBaseVM(UObject* Outer) const override;
};

UCLASS(Blueprintable)
class PROJECTLORD_API UTreeBackAction : public USelectionAction
{
    GENERATED_BODY()

public:

    virtual bool CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const override
    {
        return true;
    }


protected:

    virtual UVMSelectionAction* ConstructBaseVM(UObject* Outer) const override;

};

UCLASS(Blueprintable)
class PROJECTLORD_API UTreeCompositeBuildingPurchase : public UBuildingBasedPurchase
{
    GENERATED_BODY()

public:

    virtual bool IsHidden_Implementation(const FSelectionActionContext& Context) const override;
    virtual bool CanPerform_Implementation(const FSelectionActionContext& Context, ESelectionActionFailureReason& ReasonOut) const override;
    virtual bool Perform_Implementation(const FSelectionActionContext& Context) const override;

    USelectionAction* GetCurrentAction(const FSelectionActionContext& Context) const;

    virtual UVMSelectionAction* MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& Context) const override;

protected:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    TArray<UBuildingBasedPurchase*> Actions;

    virtual UVMSelectionAction* ConstructBaseVM(UObject* Outer) const override;

};

UCLASS(BlueprintType)
class PROJECTLORD_API UVMTreePageAction : public UVMSelectionAction
{
    GENERATED_BODY()

public:

    virtual void Perform() override;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    FString Page;

};

UCLASS(BlueprintType)
class PROJECTLORD_API UVMTreeBackAction : public UVMSelectionAction
{
    GENERATED_BODY()

public:

    virtual void Perform() override;

};

UCLASS(BlueprintType)
class PROJECTLORD_API UVMCompositeSelectionAction : public UVMSelectionAction
{
    GENERATED_BODY()

public:

    virtual void Perform() override;
    virtual void OnShown() override;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    TObjectPtr<const UTreeCompositeBuildingPurchase> CompositeAction;

};
