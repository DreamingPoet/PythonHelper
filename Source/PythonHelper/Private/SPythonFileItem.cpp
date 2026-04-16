// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#include "SPythonFileItem.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Images/SImage.h"
#include "PythonHelperStyle.h"


void SPythonFileItem::Construct(const FArguments& Args)
{
	FileTree = Args._TheBrowserElement;
	PythonFileBrowser = Args._TheBrowser;
	OnSelectStateChanged = Args._OnSelectStateChanged;

	const FSlateBrush* Brush = FileTree->Type == EBrowserTreeType::File ? FPythonHelperStyle::Get().GetBrush("PythonHelper.FileIcon") : FPythonHelperStyle::Get().GetBrush("PythonHelper.FolderIcon");
	EVisibility bShowButton = FileTree->Type == EBrowserTreeType::File ? EVisibility::Visible : EVisibility::Collapsed;
	ChildSlot[
		SAssignNew(SelectedBorder, SBorder)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(1.0f)
			.AutoWidth()
			[
				SNew(SImage)
				.Image(Brush)
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(FMargin(5,1,1,1))
			.AutoWidth()
			.FillWidth(1.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FileTree->Name))
			]

			+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(FMargin(0, 1, 10, 1))
				[
					SNew(SButton)
					.Visibility(bShowButton)
					.ButtonStyle(&FPythonHelperStyle::Get().GetWidgetStyle<FButtonStyle>("PythonHelper.Button.RunScript"))
					.ToolTipText(FText::FromString("Run this file!"))
					.OnClicked(this, &SPythonFileItem::OnRunButtonClicked)
				]
		]
	];

	SetSelected(false);
}


void SPythonFileItem::SetSelected(const bool& bSelected)
{
	bIsSelected = bSelected;
	if (bSelected)
	{
		SelectedBorder->SetBorderBackgroundColor(FLinearColor::White);
	}
	else
	{
		SelectedBorder->SetBorderBackgroundColor(FLinearColor::Transparent);
	}
}

FReply SPythonFileItem::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		PythonFileBrowser->ClearSelection();
		SetSelected(true);
		OnSelectStateChanged.ExecuteIfBound(FileTree);

		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}


FReply SPythonFileItem::OnRunButtonClicked()
{

	FString FilePath = FileTree->Path;

	if (FPaths::FileExists(FilePath))
	{
		const FString Cmd = "py \"" + FilePath + "\"";
		GEngine->Exec(GEngine->GetWorld(), *Cmd);
	}
	return FReply::Handled();
}