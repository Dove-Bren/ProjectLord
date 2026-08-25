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

	if (bUpdateHistory && !PageName.IsEmpty())
	{
		PageHistory.Add(PageName);
	}
	UpdateActions(PageOpt->Actions);
	return true;
}

bool UTreePageAction::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	/*auto TreeVM = Context.Selection->GetActionTreeVM();
	return TreeVM && TreeVM->HasPage(GetPage());*/
	return true;
}

bool UTreePageAction::Perform_Implementation(const FSelectionActionContext& Context)
{
	auto TreeVM = Context.Selection->GetActionTreeVM();
	return TreeVM->GoToPage(GetPage());
}

bool UTreeBackAction::CanPerform_Implementation(const FSelectionActionContext& Context) const
{
	/*auto TreeVM = Context.Selection->GetActionTreeVM();
	return TreeVM && !TreeVM->IsAtRoot();*/
	return true;
}

bool UTreeBackAction::Perform_Implementation(const FSelectionActionContext& Context)
{
	auto TreeVM = Context.Selection->GetActionTreeVM();
	return TreeVM->GoBack();
}
