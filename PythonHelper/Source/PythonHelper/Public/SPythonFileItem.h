// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "SPythonFileBrowser.h"


class SPythonFileItem : public SCompoundWidget 
{
public:

	SLATE_BEGIN_ARGS(SPythonFileItem)
	{}

	SLATE_ARGUMENT(TSharedPtr<class SPythonFileBrowser>, TheBrowser)
	SLATE_ARGUMENT(TSharedPtr<FBrowserElement>, TheBrowserElement)
	SLATE_EVENT(FOnElementSelected, OnSelectStateChanged)

	SLATE_END_ARGS()

	void SetSelected(const bool& bSelected);

private:

	// SWidget
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	TSharedPtr<FBrowserElement> FileTree;

	TSharedPtr<class SPythonFileBrowser> PythonFileBrowser;

	FOnElementSelected OnSelectStateChanged;


	FReply OnRunButtonClicked();

	bool bIsSelected = false;

	TSharedPtr<class SBorder> SelectedBorder;


public:
	void Construct(const FArguments& Args);
};

