// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#include "SPythonFileBrowser.h"
#include "Widgets/Images/SImage.h"
#include "HAL/PlatformFilemanager.h"
#include "PythonHelperStyle.h"
#include "SPythonFileItem.h"
#include "PythonHelper.h"
#include "Widgets/Layout/SScrollBox.h"
#include "EditorScriptingUtilities/Public/EditorLevelLibrary.h"
#include "Misc/FileHelper.h"



void SPythonFileBrowser::OnSelectStateChanged(TSharedPtr<FBrowserElement> Item)
{
	LastSelection = Item;
	OnElementSelected.ExecuteIfBound(LastSelection);
}

void SPythonFileBrowser::Construct(const FArguments& Args)
{
	OnElementSelected = Args._OnElementSelected;

	ChildSlot[

				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Fill)
				.Padding(3)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					.VAlign(VAlign_Fill)
					[
						SAssignNew(Container, SVerticalBox)
						+ SVerticalBox::Slot()
						.VAlign(VAlign_Fill)
						[
							SAssignNew(FileTreeView, STreeView<TSharedPtr<FBrowserElement>>)
							.TreeItemsSource(&RootDirectory)
							.SelectionMode(ESelectionMode::Single)
							.OnGenerateRow(this, &SPythonFileBrowser::OnGenerateRow)
							.OnGetChildren(this, &SPythonFileBrowser::OnGetChildren)
						]
					]
				]

	];

	BrowserUpdate();
}

void SPythonFileBrowser::GenerateChildrenTree(TSharedPtr<FBrowserElement> TheBrowserElement, TArray<TSharedPtr<FBrowserElement>>& LinearOut)
{
	if (FPaths::DirectoryExists(TheBrowserElement->Path) && TheBrowserElement.IsValid())
	{
		FString Left;
		TheBrowserElement->Path.Split("/", &Left, nullptr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		Left.Split("/", nullptr, &TheBrowserElement->Name, ESearchCase::IgnoreCase, ESearchDir::FromEnd);


		TArray<FString> Folders;
		TArray<FString> Files;
		FString FinalPath = TheBrowserElement->Path / TEXT("*");

		IFileManager& FileManager = IFileManager::Get();
		FileManager.FindFiles(Folders, *FinalPath, false, true);
		FileManager.FindFiles(Files, *FinalPath, true, false);

		// In the process of scanning the entire directory, clean up only at the root directory
		// In the recursive function, please carefully examine the temporary variables!!!
		if (TheBrowserElement->Type == EBrowserTreeType::Root){
			LastUnsupportedFolders = UnsupportedFolders;
			LastUnsupportedFiles = UnsupportedFiles;
			UnsupportedFolders.Empty();
			UnsupportedFiles.Empty();
		}
		
		for (FString Folder : Folders)
		{
			
			if (Folder == "__pycache__" || Folder == "unreallib" ) continue;
			
			if (Folder == "test" || !FPythonHelperModule::IsFileOrFoldNameValid( Folder) )
			{	
				if (LastUnsupportedFolders.Find(TheBrowserElement->Path / Folder) == INDEX_NONE)
				{
					UE_LOG(LogPythonHelper, Warning, TEXT("Not supported folder or directory name \"%s\" in \"%s\" !"), *Folder, *TheBrowserElement->Path);
				}
				UnsupportedFolders.Add(TheBrowserElement->Path / Folder);
				continue;
			}
			
			FString FullFolder = TheBrowserElement->Path + Folder + "/";
			TSharedPtr<FBrowserElement> NewRoot = MakeShareable(new FBrowserElement());
			NewRoot->Path = FullFolder;
			GenerateChildrenTree(NewRoot, LinearOut);
			
			TheBrowserElement->AddChild(NewRoot, TheBrowserElement);
			LinearOut.Add(NewRoot);
		}

		for (FString File : Files)
		{
			if (!File.EndsWith(".py") || File == "unreallib.py" || File == "__init__.py") continue;

			if ( File == "test.py" || !FPythonHelperModule::IsFileOrFoldNameValid( File.LeftChop(3)) )
			{
				if (LastUnsupportedFiles.Find(TheBrowserElement->Path / File) == INDEX_NONE)
				{
					UE_LOG(LogPythonHelper, Warning, TEXT("Not supported file name \"%s\" in \"%s\" !"), *File, *TheBrowserElement->Path);
				}
				UnsupportedFiles.Add(TheBrowserElement->Path / File);
				continue;
			}

			TSharedPtr<FBrowserElement> NewRoot = MakeShareable(new FBrowserElement());
			NewRoot->Type = EBrowserTreeType::File;
			NewRoot->Name = File;
			NewRoot->Path = TheBrowserElement->Path + File;

			FString Code;
			FFileHelper::LoadFileToString(Code, *NewRoot->Path);

			NewRoot->MD5 = FMD5::HashAnsiString(*Code);

			TheBrowserElement->AddChild(NewRoot, TheBrowserElement);
			LinearOut.Add(NewRoot);
		}
		
	}
}

void SPythonFileBrowser::BrowserUpdate()
{
	TArray<TSharedPtr<FBrowserElement>> CurrentAllElements;
	TSharedPtr<FBrowserElement> ProjectRoot = MakeShareable(new FBrowserElement());
	TSharedPtr<FBrowserElement> MainRoot = MakeShareable(new FBrowserElement());

	CurrentAllElements.Add(ProjectRoot);
	CurrentAllElements.Add(MainRoot);

	ProjectRoot->Path = FPythonHelperModule::GetPythonDirectory() + "/";
	ProjectRoot->Type = EBrowserTreeType::Root;
	GenerateChildrenTree(ProjectRoot, CurrentAllElements);

	auto CompareElements = [](TArray<TSharedPtr<FBrowserElement>> A, TArray<TSharedPtr<FBrowserElement>> B) ->bool
	{
		if (A.Num() != B.Num()) return false;
		for(int32 i =0; i < A.Num(); ++i)
		{
			if (*A[i] != *B[i])
			{
				return false;
			}
		}
		return true;
	};

	if (CompareElements(CurrentAllElements, AllElements))
	{
		return;
	}else
	{	
		AllElements = CurrentAllElements;
		UE_LOG(LogPythonHelper, Log, TEXT("Python file browser updated! "));
	}

	ProjectRoot->Name = TEXT("Project/Python");

	MainRoot->AddChild(ProjectRoot, MainRoot);

	FileTreeView->GetExpandedItems(ExpandedItems);

	Items.Empty();
	RootDirectory = MainRoot->Children;
	FileTreeView->RequestTreeRefresh();

	for (auto& Elem : AllElements)
	{
		for (auto& Expanded : ExpandedItems)
		{
			if (Elem->Name == Expanded->Name && Elem->Path == Expanded->Path)
			{
				FileTreeView->SetItemExpansion(Elem, true);
				continue;
			}
		}
	}
}

void SPythonFileBrowser::TransformScriptToString(TSharedPtr<FBrowserElement> Script, OUT TArray< FString >& Array)
{
	Array.Add(Script->Name);
}

void SPythonFileBrowser::RebuldTreeView()
{
	float OldScrollOffset = 0.0f;
	if (FileTreeView.IsValid())
	{
		OldScrollOffset = FileTreeView->GetScrollOffset();
	}

}

TSharedRef<ITableRow> SPythonFileBrowser::OnGenerateRow(TSharedPtr<FBrowserElement> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	check(Item.IsValid());

	TSharedPtr<SPythonFileItem> BrowserItem = SNew(SPythonFileItem)
		.TheBrowserElement(Item)
		.OnSelectStateChanged(this, &SPythonFileBrowser::OnSelectStateChanged)
		.TheBrowser(SharedThis(this));

	Items.Add(BrowserItem);

	return 	SNew(STableRow<TSharedPtr<FBrowserElement>>, OwnerTable)
		.Padding(0)
		[
			BrowserItem->AsShared()
		];
}

void SPythonFileBrowser::OnGetChildren(TSharedPtr<FBrowserElement> Item, TArray<TSharedPtr<FBrowserElement>>& OutChildren)
{
	check(Item.IsValid());
	OutChildren = Item->Children;
}

TSharedPtr<FBrowserElement> SPythonFileBrowser::GetLastSelection()
{
	return LastSelection;
}

void SPythonFileBrowser::ClearSelection()
{
	FileTreeView->ClearSelection();
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].IsValid())
		{
			Items[i]->SetSelected(false);
		}
	}
}
