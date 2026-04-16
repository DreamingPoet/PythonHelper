// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"

class SPythonFileItem;

enum EBrowserTreeType
{
	None,
	Root,
	Directory,
	File
};

class FBrowserElement
{

public:
	FString Name;

	EBrowserTreeType Type = EBrowserTreeType::Directory;;

	FString Path;

	TArray<TSharedPtr<FBrowserElement>> Children;

	TSharedPtr<FBrowserElement> Parent;

	/** python file MD5, used to detect file changes */
	FString MD5;

	FBrowserElement() {
	};

	void AddChild(TSharedPtr<FBrowserElement>& Child, TSharedPtr<FBrowserElement>& InParent)
	{
		Child->Parent = InParent;
		Children.Add(Child);
	}


	/**
	 * Equality operator.
	 *
	 * @param Other to compare.
	 * @returns True if this is the same as Other. False otherwise.
	 */
	bool operator==(const FBrowserElement& Other) const
	{
		return Type == Other.Type && Path == Other.Path;
	}

	/**
	 * Inequality operator.
	 *
	 * @param Other to compare.
	 * @returns True if this is NOT the same as Other. False otherwise.
	 */
	FORCEINLINE bool operator!=(const FBrowserElement& Other) const
	{
		return !(*this == Other);
	}
};


DECLARE_DELEGATE_OneParam(FOnElementSelected, TSharedPtr<FBrowserElement>)


class SPythonFileBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPythonFileBrowser)
	{}

	SLATE_EVENT(FOnElementSelected, OnElementSelected)
	SLATE_END_ARGS()

private:

	void OnExpansionChanged(TSharedPtr<FBrowserElement> Item, bool bValue);

	void TransformScriptToString(TSharedPtr<FBrowserElement> Script, OUT TArray< FString >& Array);

	void RebuldTreeView();

	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FBrowserElement> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnGetChildren(TSharedPtr<FBrowserElement> Item, TArray<TSharedPtr<FBrowserElement>>& OutChildren);

	void OnSelectStateChanged(TSharedPtr<FBrowserElement> Item);

	void GenerateChildrenTree(TSharedPtr<FBrowserElement> TheBrowserElement, TArray<TSharedPtr<FBrowserElement>>& LinearOut);

public:

	void Construct(const FArguments& Args);

	void BrowserUpdate();

	TSharedPtr<FBrowserElement> GetLastSelection();

	void ClearSelection();

private:
	FOnElementSelected OnElementSelected;

	TSharedPtr<FBrowserElement> LastSelection;

	TSharedPtr<STreeView<TSharedPtr<FBrowserElement>>> FileTreeView;

	TArray<TSharedPtr<FBrowserElement>> RootDirectory;

	TSharedPtr<SVerticalBox> Container;

	TArray<TSharedPtr<FBrowserElement>> AllElements;

	TArray<TSharedPtr<class SPythonFileItem>> Items;

	TSet<TSharedPtr<FBrowserElement>> ExpandedItems;

	TArray<FString> UnsupportedFolders;

	TArray<FString> UnsupportedFiles;

	TArray<FString> LastUnsupportedFolders;
	
	TArray<FString> LastUnsupportedFiles;


};
