// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/SelectionActionTreeViewModel.h"

#include "LordLogging.h"
#include "Gameplay/Interaction/SelectionComponent.h"
#include "UI/ViewModels/SelectionActionViewModel.h"

const FString UVMSelectionActionTree::RootPageName = TEXT("");

UVMSelectionActionTree::UVMSelectionActionTree()
{
}

void UVMSelectionActionTree::Reset()
{
	Pages.Empty();
	PageHistory.Empty();
	UpdateActions({});
}

void UVMSelectionActionTree::AddPage(FString PageName, TArray<UVMSelectionAction*> PageActions)
{
	const FString& NameToUse = (PageName.IsEmpty() ? RootPageName : PageName);
	if (!ensure(!HasPage(NameToUse)))
	{
		UE_LOG(LordUI, Error, TEXT("Attempted to overwrite page %s in action tree"), *NameToUse);
		return;
	}

	FPage Page;
	Page.Actions = MoveTemp(PageActions);
	Pages.Add(NameToUse, MoveTemp(Page));

	if (NameToUse == RootPageName)
	{
		GoToPageInternal(RootPageName, false);
	}
}

bool UVMSelectionActionTree::GoToPage(const FString& PageName)
{
	return GoToPageInternal(PageName, true);
}

bool UVMSelectionActionTree::GoBack()
{
	if (!PageHistory.IsEmpty())
	{
		PageHistory.Pop();

		FString NewPage = PageHistory.IsEmpty() ? RootPageName : PageHistory.Last();
		GoToPageInternal(NewPage, false);

		return true;
	}
	return false;
}

bool UVMSelectionActionTree::GoToRoot()
{
	if (!IsAtRoot())
	{
		GoToPageInternal(RootPageName, true);
		return true;
	}
	return false;
}

const FString& UVMSelectionActionTree::GetCurrentPage() const
{
	return PageHistory.IsEmpty() ? RootPageName : PageHistory.Last();
}

FPage* UVMSelectionActionTree::FindPage(const FString& PageName)
{
	return Pages.Find(PageName);
}

const FPage* UVMSelectionActionTree::FindPage(const FString& PageName) const
{
	return Pages.Find(PageName);
}

bool UVMSelectionActionTree::GoToPageInternal(const FString& PageName, bool bUpdateHistory)
{
	auto PageOpt = FindPage(PageName);
	if (!PageOpt)
	{
		return false;
	}

	if (bUpdateHistory)
	{
		if (PageName == RootPageName)
		{
			PageHistory.Empty();
		}
		else
		{
			PageHistory.Add(PageName);
		}
	}
	UpdateActions(PageOpt->Actions);
	return true;
}

void UVMSelectionActionTree::UpdateActions(TArray<UVMSelectionAction*> NewActions)
{
	CurrentActions = MoveTemp(NewActions);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CurrentActions);
}

void UVMSelectionActionTree::RefreshPage()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CurrentActions);
}

UVMSelectionAction* UTreePageAction::ConstructBaseVM(UObject* Outer) const
{
	auto VM = CreateLordVM<UVMTreePageAction>(Outer);
	VM->Init(this);
	VM->Page = GetPage();
	// Should be setting name and desc?
	return VM;
}

void UVMTreePageAction::Perform()
{
	// Don't need to call super / actually call action. Operate on tree.
	if (ensure(ActionTree))
	{
		ActionTree->GoToPage(Page);
	}
}

UVMSelectionAction* UTreeBackAction::ConstructBaseVM(UObject* Outer) const
{
	auto VM = CreateLordVM<UVMTreeBackAction>(Outer);
	VM->Init(this);
	// Should be setting name and desc?
	return VM;
}

void UVMTreeBackAction::Perform()
{
	// Don't need to call super / actually call action. Operate on tree.
	if (ensure(ActionTree))
	{
		ActionTree->GoBack();
	}
}

UVMSelectionAction* UTreeCompositeBuildingPurchase::ConstructBaseVM(UObject* Outer) const
{
	auto VM = CreateLordVM<UVMCompositeSelectionAction>(Outer);
	VM->Init(this);
	// Should be setting name and desc?
	return VM;
}

UVMSelectionAction* UTreeCompositeBuildingPurchase::MakeViewModel_Implementation(UObject* Outer, const FSelectionActionContext& InContext) const
{
	auto ViewModel = Super::MakeViewModel_Implementation(Outer, InContext);

	Cast<UVMCompositeSelectionAction>(ViewModel)->CompositeAction = this;

	// Almost wish this func could visit each action and register a callback for when it thinks its VM should refresh at all.

	return ViewModel;
}

bool UTreeCompositeBuildingPurchase::IsHidden_Implementation(const FSelectionActionContext& InContext) const
{
	auto Action = GetCurrentAction(InContext);
	if (Action)
	{
		return Action->IsHidden(InContext);
	}
	return true;
}

bool UTreeCompositeBuildingPurchase::CanPerform_Implementation(const FSelectionActionContext& InContext, ESelectionActionFailureReason& ReasonOut) const
{
	auto Action = GetCurrentAction(InContext);
	if (Action)
	{
		return Action->CanPerform(InContext, ReasonOut);
	}
	return false;
}

bool UTreeCompositeBuildingPurchase::Perform_Implementation(const FSelectionActionContext& InContext) const
{
	auto Action = GetCurrentAction(InContext);
	if (Action)
	{
		return Action->Perform(InContext);
	}
	return false;
}

USelectionAction* UTreeCompositeBuildingPurchase::GetCurrentAction(const FSelectionActionContext& InContext) const
{
	for (auto Action : Actions)
	{
		if (!Action->IsHidden(InContext))
		{
			return Action;
		}
	}

	return nullptr;
}

void UVMCompositeSelectionAction::Perform()
{
	Super::Perform();

	OnShown();
}

void UVMCompositeSelectionAction::OnShown()
{
	// Manually poll and refresh since base action doesn't have listeners
	// Update our values
	auto Context = GetContext();
	auto RealAction = CompositeAction->GetCurrentAction(Context);

	if (!RealAction)
	{
		SetHidden(true);
	}
	else
	{
		ESelectionActionFailureReason Reason;
		bool bInEnabled = RealAction->CanPerform(Context, Reason);
		SetName(RealAction->GetName());
		SetDescription(RealAction->GetDescription());
		SetEnabled(bInEnabled, Reason);
		SetHidden(false); // Wouldn't be in this block otherwise
		if (auto CostAction = Cast<USelectionPurchase>(RealAction))
		{
			SetGoldCost(CostAction->GetGoldCost(Context));
		}

		SetIcon(RealAction->GetIcon());
		OverrideAction(RealAction);
	}

	Super::OnShown();
}
