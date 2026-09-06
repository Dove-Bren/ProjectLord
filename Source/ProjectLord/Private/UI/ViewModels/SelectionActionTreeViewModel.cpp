// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/SelectionActionTreeViewModel.h"

#include "LordLogging.h"
#include "Gameplay/SelectionComponent.h"
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

bool UTreePageAction::CanPerform_Implementation() const
{
	/*auto TreeVM = Context.ActionTree;
	return TreeVM && TreeVM->HasPage(GetPage());*/
	return true;
}

bool UTreePageAction::Perform_Implementation()
{
	auto TreeVM = Context.ActionTree;
	return TreeVM->GoToPage(GetPage());
}

bool UTreeBackAction::CanPerform_Implementation() const
{
	/*auto TreeVM = Context.ActionTree;
	return TreeVM && !TreeVM->IsAtRoot();*/
	return true;
}

bool UTreeBackAction::Perform_Implementation()
{
	auto TreeVM = Context.ActionTree;
	return TreeVM->GoBack();
}

void UTreeCompositeBuildingPurchase::Setup(const FSelectionActionContext& InContext)
{
	Super::Setup(InContext);

	for (auto ActionClass : Actions)
	{
		auto Action = NewObject<UBuildingBasedPurchase>(this, ActionClass);
		Action->Setup(Context);
		ActionInstances.Add(Action);
	}

	RefreshToShow();
}

bool UTreeCompositeBuildingPurchase::IsHidden_Implementation() const
{
	auto Action = GetCurrentAction();
	if (Action)
	{
		return Action->IsHidden();
	}
	return true;
}

bool UTreeCompositeBuildingPurchase::CanPerform_Implementation() const
{
	auto Action = GetCurrentAction();
	if (Action)
	{
		return Action->CanPerform();
	}
	return false;
}

bool UTreeCompositeBuildingPurchase::Perform_Implementation()
{
	auto Action = GetCurrentAction();
	if (Action)
	{
		return Action->Perform();
	}
	return false;
}

void UTreeCompositeBuildingPurchase::RefreshToShow_Implementation()
{
	CurrentAction = nullptr;
	for (auto Action : ActionInstances)
	{
		if (!Action->IsHidden())
		{
			CurrentAction = Action;

			// Update our values
			Name = CurrentAction->GetName();
			Description = CurrentAction->GetDescription();
			Icon = CurrentAction->GetIcon();

			// Update VM (ugly code dupe from VM internal)
			ViewModel->SetName(Name);
			ViewModel->SetDescription(Description);
			ViewModel->SetEnabled(CurrentAction->CanPerform());
			ViewModel->SetHidden(false); // Wouldn't be in this block otherwise
			if (auto CostAction = Cast<USelectionPurchase>(CurrentAction))
			{
				ViewModel->SetGoldCost(CostAction->GetGoldCost());
			}

			ViewModel->SetIcon(CurrentAction->GetIcon());
			ViewModel->OverrideAction(CurrentAction);

			break;
		}
	}

	if (!CurrentAction)
	{
		ViewModel->SetHidden(true);
	}
	Super::RefreshToShow_Implementation();
}

USelectionAction* UTreeCompositeBuildingPurchase::GetCurrentAction() const
{
	return CurrentAction;
}
