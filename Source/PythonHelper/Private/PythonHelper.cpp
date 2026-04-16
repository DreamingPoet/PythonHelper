// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#include "PythonHelper.h"
#include "PythonHelperStyle.h"
#include "PythonHelperCommands.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include <SPythonFileBrowser.h>
#include <Widgets/Input/SButton.h>
#include "SPythonFileBrowser.h"
#include <Misc/FileHelper.h>
#include <SPythonFuncBox.h>
#include <Widgets/Layout/SScrollBox.h>
#include "Internationalization/Regex.h"
#include "IPythonScriptPlugin.h"
#include "Interfaces/IPluginManager.h"

#include "Editor.h"
#include "TimerManager.h"
#include "Widgets/Input/SCheckBox.h"


DEFINE_LOG_CATEGORY(LogPythonHelper)

static const FName PythonHelperTabName("PythonHelper");

#define LOCTEXT_NAMESPACE "FPythonHelperModule"

FString FPythonHelperModule::PluginDirectory = "";

void FPythonHelperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPythonHelperStyle::Initialize();
	FPythonHelperStyle::ReloadTextures();

	//Create scripts directory
	CheckDependencies();

	FPythonHelperCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FPythonHelperCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FPythonHelperModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPythonHelperModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(PythonHelperTabName, FOnSpawnTab::CreateRaw(this, &FPythonHelperModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FPythonHelperTabTitle", "PythonHelper"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	IPythonScriptPlugin::Get()->OnPythonInitialized().AddRaw(this, &FPythonHelperModule::OnPythonInitialized);
}

void FPythonHelperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FPythonHelperStyle::Shutdown();

	FPythonHelperCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PythonHelperTabName);
}


TSharedRef<SDockTab> FPythonHelperModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	if (TimerHander.IsValid())
	{
		GEditor->GetTimerManager()->UnPauseTimer(TimerHander);
	}

	TSharedPtr<SDockTab> tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.OnTabClosed_Raw(this, &FPythonHelperModule::OnTabClosed)
		[
		// Put your tab content here!
		SNew(SBox) // Outer border
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SSplitter) // Inner horizontal box
			//.Style(FPythonHelperStyle::Get(), "PythonHelper.Splitter")
			+ SSplitter::Slot() // Horizontal box element 1
			[ 
				SNew(SBorder) // Vertical box containing the open file button and file browser
				.BorderBackgroundColor(FColor(128, 128, 128))
				[		
					
					SNew(SVerticalBox) // Vertical box containing the open file button and file browser
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Left)
					.Padding(10)
					[

						SNew(SVerticalBox) // Vertical box containing the open file button and button text
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SNew(SButton)
							.ButtonStyle(&FPythonHelperStyle::Get().GetWidgetStyle<FButtonStyle>("PythonHelper.Button.OpenPythonFolder"))
							.ToolTipText(FText::FromString("Open project python folder!"))
							.OnClicked_Lambda([] {
								// Open folder
								FPlatformProcess::ExploreFolder(*FPythonHelperModule::GetPythonDirectory());
								return FReply::Handled();
								})
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock).Text(FText::FromString("Open Folder")).ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
						]
					]
					+ SVerticalBox::Slot() // Separator line
					.AutoHeight()
					.HAlign(HAlign_Fill)
					[
						SNew(SImage).Image(FPythonHelperStyle::Get().GetBrush("PythonHelper.White1x"))
						.ColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 1)))
					]
					+ SVerticalBox::Slot() 
					.VAlign(VAlign_Fill)
					[
						SAssignNew(PythonFileBrowser, SPythonFileBrowser) // FileBrowser
						.OnElementSelected_Raw(this, &FPythonHelperModule::GenerateFunctionButtons)
					]
				]

			]
			+ SSplitter::Slot() // Horizontal box element 2
			[	
				SNew(SBorder)
				.BorderBackgroundColor(FColor(128, 128, 128))
				.VAlign(VAlign_Top)
				[	
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()// python file function tittle
					.AutoHeight()
					.Padding(FMargin(0,2,0,5))
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot() // python file name
						.HAlign(HAlign_Left)
						[
							SAssignNew(PythonFilenameText, STextBlock)
						]
						+ SHorizontalBox::Slot() // refresh icon
						.HAlign(HAlign_Right)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SImage).Image(FPythonHelperStyle::Get().GetBrush("PythonHelper.Refresh"))
							]
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(FMargin(1, 0, 2, 0))
							[
								SNew(STextBlock).Text(FText::FromString("Auto Refresh")).ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
							]
							+ SHorizontalBox::Slot() // auto refresh
							.AutoWidth()
							[
								SNew(SCheckBox)
								.IsChecked(true)
								.ToolTipText(FText::FromString("If true, automatically detect the change of the file and refresh the list of callable functions!"))
								.OnCheckStateChanged_Raw(this, &FPythonHelperModule::OnAutoRefreshPythonFile)
							]
						]

					]
					+ SVerticalBox::Slot() // python file function
					[
						SNew(SScrollBox)
						+ SScrollBox::Slot()
						[
							SAssignNew(FunctionContainer, SVerticalBox)
						]
					]

				]
			]

		]
	];
	
	tab->SetTabIcon(FPythonHelperStyle::Get().GetBrush("PythonHelper.PythonIcon16"));
	return tab.ToSharedRef();
}

void FPythonHelperModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(PythonHelperTabName);
}

void FPythonHelperModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("PythonHelper");
			Section.AddMenuEntryWithCommandList(
			FPythonHelperCommands::Get().OpenPluginWindow,
			PluginCommands,
			TAttribute<FText>(),
			TAttribute<FText>(),
			FSlateIcon(FPythonHelperStyle::GetStyleSetName(), "PythonHelper.PythonIcon16")
			);
		}

		UToolMenus* Menus = UToolMenus::Get();
		{
			Menus->SetSectionLabel("LevelEditor.MainMenu.Window","PythonHelper",FText::FromString("PythonHelper") );
		}

	}
	// Add a timer to detect file changes in the folder
	GEditor->GetTimerManager()->SetTimer(TimerHander, FTimerDelegate::CreateRaw(this, &FPythonHelperModule::TimerCall, false), 1.5, true);
}


void FPythonHelperModule::GenerateFunctions(const FString Code, const FString Path)
{
	FunctionContainer->ClearChildren();

	// def say_hello():
	FRegexPattern Pattern(TEXT("def [^0-9][a-zA-Z_]*\\(\\):"));

	// def say_hello(): ##alias=你好##
	FRegexPattern PatternFuncAlias(TEXT("def [^0-9][^\\n]*\\(\\): ##alias=[^0-9][^\\s]*##"));

	// All functions and their aliases
	TMap<FString, FString> Functions;

	// All comments
	TArray<FString> Comments;
	TArray<FString> AllLines;

	// Get all comments
	Code.ParseIntoArrayLines(AllLines);
	for (auto i : AllLines)
	{
		// Get all line comments
		FString a = i.TrimStart();
		if (a != "" && a[0] == *"#")
		{
			Comments.Add(a);
		}

		// Get all functions
		FRegexMatcher MacherFuncAlias(PatternFuncAlias, a);

		bool AlaisFound = false;
		// Find functions with aliases
		while (MacherFuncAlias.FindNext())
		{
			FString result = MacherFuncAlias.GetCaptureGroup(0);
			// def say_hello(): ##alias=你好##
			FString LeftS;
			FString RightS;
			result.Split("##alias=", &LeftS, &RightS);

			Functions.Add(LeftS.LeftChop(2).RightChop(4), RightS.LeftChop(2));
			AlaisFound = true;
		}

		// If no function with alias is found
		if(!AlaisFound)
		{
			FRegexMatcher Macher(Pattern, a);
			while (Macher.FindNext())
			{
				FString result = Macher.GetCaptureGroup(0);
				result = result.LeftChop(1).RightChop(4);
				Functions.Add(result, "");
			}
			
		}


	}

	// Get all block comments
	FRegexPattern PatternComment1(TEXT("'{3}[\\s\\S]*?'{3}"));
	FRegexMatcher MacherComment1(PatternComment1, Code);
	while (MacherComment1.FindNext())
	{
		FString result = MacherComment1.GetCaptureGroup(0);
		Comments.Add(result);
	}
	// Get all block comments
	FRegexPattern PatternComment2(TEXT("\"{3}[\\s\\S]*?\"{3}"));
	FRegexMatcher MacherComment2(PatternComment2, Code);
	while (MacherComment2.FindNext())
	{
		FString result = MacherComment2.GetCaptureGroup(0);
		Comments.Add(result);
	}

	for (auto Func : Functions)
	{
		bool bCommented = false;
		for (auto i : Comments)
		{
			if (i.Contains(Func.Key))
			{
				bCommented = true;
				break;
			}
		}
		if (bCommented) continue;

		FunctionContainer->AddSlot()
			.VAlign(VAlign_Top)
			.Padding(0, 0, 0, 4)
			[
				SNew(SPythonFuncBox)
				.ToolTipText(FText::FromString(Func.Key))
				.FullPath(Path)
				.FunctionName(Func.Key)
				.Alias(Func.Value)
			];
	}
}

FString FPythonHelperModule::GetFilePathCharater(const FString Path)
{
	FString filePath = FPaths::GetPath(Path);
	FString basefileName = FPaths::GetBaseFilename(Path);

	FString PythonDirectory = *FPythonHelperModule::GetPythonDirectory();

	filePath = filePath.Replace(*PythonDirectory, TEXT(""));
	filePath = filePath.Replace(TEXT("/"), TEXT(".")).RightChop(1);

	// Get import as
	FString importas = filePath.Replace(TEXT("."), TEXT("")) + basefileName;

	return importas;
}

void FPythonHelperModule::GenerateFunctionButtons(TSharedPtr<FBrowserElement> Item)
{
	if (Item->Type == EBrowserTreeType::File)
	{

		CurrentItem = Item;
		FString result;
		FString filePath = FPaths::GetPath(Item->Path);
		FString basefileName = FPaths::GetBaseFilename(Item->Path);
		FString PythonDirectory = *FPythonHelperModule::GetPythonDirectory();
		filePath = filePath.Replace(*PythonDirectory, TEXT(""));
		filePath = filePath.Replace(TEXT("/"), TEXT(".")).RightChop(1);

		// Get import as
		FString importas = filePath.Replace(TEXT("."), TEXT("")) + basefileName;
		if (filePath == "")
		{
			result = "import " + basefileName + " as " + importas;
		}
		else
		{
			result = "import " + filePath + "." + basefileName + " as " + importas;
		}

		IPythonScriptPlugin::Get()->ExecPythonCommand(*result);
		PythonFilenameText->SetText(FText::FromString("Functions in " + Item->Name));

		FString Code;
		FFileHelper::LoadFileToString(Code, *Item->Path);

		GenerateFunctions(Code, Item->Path);

	}
	else
	{
		CurrentItem = nullptr;
		PythonFilenameText->SetText(FText::FromString(""));
		FunctionContainer->ClearChildren();
	}
}


void FPythonHelperModule::OnAutoRefreshPythonFile(ECheckBoxState NewState)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		bAutoRefresh = false;
		UE_LOG(LogPythonHelper, Log, TEXT("Stop automatically updating files!"));
		break;
	case ECheckBoxState::Checked:
		bAutoRefresh = true;
		UE_LOG(LogPythonHelper, Log, TEXT("Start automatically updating files..."));
		break;
	}
}


void FPythonHelperModule::TimerCall(bool bForceEndChange /*= false*/)
{
	if (PythonFileBrowser)
	{
		PythonFileBrowser->BrowserUpdate();
	}

	if (bAutoRefresh && CurrentItem)
	{
		FString Code;
		if( !FPaths::FileExists(*CurrentItem->Path) ) return;

		FFileHelper::LoadFileToString(Code, *CurrentItem->Path);
		FString TempMD5 = FMD5::HashAnsiString(*Code);
		if (TempMD5 != *CurrentItem->MD5)
		{
			CurrentItem->MD5 = TempMD5;
			GenerateFunctions(Code, *CurrentItem->Path);
#if PY_MAJOR_VERSION < 3

			// python2 ,  directly reload()
			FString reload = "reload(" + FPythonHelperModule::GetFilePathCharater(*CurrentItem->Path) + ")";
			IPythonScriptPlugin::Get()->ExecPythonCommand(*reload);
#else
			FString reimport = "import importlib";
			IPythonScriptPlugin::Get()->ExecPythonCommand(*reimport);
			FString reload = "importlib.reload(" + FPythonHelperModule::GetFilePathCharater(*CurrentItem->Path) + ")";
			IPythonScriptPlugin::Get()->ExecPythonCommand(*reload);
#endif
		}
	}
	//UE_LOG(LogPythonHelper, Log, TEXT("TimerCall..."));
}

void FPythonHelperModule::OnTabClosed(TSharedRef<SDockTab> Tab)
{
	if(TimerHander.IsValid())
	{
		GEditor->GetTimerManager()->PauseTimer(TimerHander);
	}
}

// Create and copy python dependencies
void FPythonHelperModule::CheckDependencies()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	TArray<TSharedRef<IPlugin>> Plugins = IPluginManager::Get().GetEnabledPlugins();
	for (const TSharedRef<IPlugin>& Plugin : Plugins)
	{
		if (Plugin->GetName() == "PythonHelper")
		{
			PluginDirectory = Plugin->GetBaseDir();
		}
	}

	if (!PlatformFile.DirectoryExists(*FPythonHelperModule::GetPythonDirectory()))
	{
		PlatformFile.CreateDirectoryTree(*FPythonHelperModule::GetPythonDirectory());
	}

	if (!PlatformFile.DirectoryExists(*(FPythonHelperModule::GetPythonDirectory()/"unreallib")))
	{
		PlatformFile.CreateDirectoryTree(*(FPythonHelperModule::GetPythonDirectory() / "unreallib"));
	}

	if ( !PlatformFile.FileExists(  *(FPythonHelperModule::GetPythonDirectory() / "unreallib/unreallib.py") ))
	{
		FString srcFile = PluginDirectory / "Resources/unreallib/unreallib.py";
		FString destFile = FPythonHelperModule::GetPythonDirectory() / "unreallib/unreallib.py";
		PlatformFile.CopyFile(*destFile, *srcFile);

	}

	if (!PlatformFile.FileExists(*(FPythonHelperModule::GetPythonDirectory() / "example.py")))
	{
		FString srcFile1 = PluginDirectory / "Resources/example.py";
		FString destFile1 = FPythonHelperModule::GetPythonDirectory() / "example.py";
		PlatformFile.CopyFile(*destFile1, *srcFile1);

	}

}

void FPythonHelperModule::OnPythonInitialized()
{
	FString PyCode =
		"import sys\n"\
		"sys.path.append(\"" + GetPythonDirectory() + "\")\n";

	bool Result = IPythonScriptPlugin::Get()->ExecPythonCommand(*PyCode);
}

bool FPythonHelperModule::IsFileOrFoldNameValid(FString Name)
{
	if (Name.IsEmpty())
		return false;

	uint32 FirstChar = uint32(Name[0]);
	uint32 char_a = uint32('a');
	uint32 char_z = uint32('z');
	uint32 char_A = uint32('A');
	uint32 char_Z = uint32('Z');
	uint32 num_0 = uint32('0');
	uint32 num_9 = uint32('9');
	uint32 char_underline = uint32('_');

	if (FirstChar >= num_0 && FirstChar <= num_9) return false;

	for (auto i : Name)
	{
		uint32 current_char = uint32(i);
		if (!((current_char >= num_0 && current_char <= num_9) ||
			(current_char >= char_a && current_char <= char_z) ||
			(current_char >= char_A && current_char <= char_Z) ||
			current_char == char_underline))
		{
			//FString str = TEXT(" ");
			//str[0] = i;
			//UE_LOG(LogPythonHelper, Log, TEXT("unicode iter %s %d "), *str , uint32(i));
			return false;
		}
	}
	return true;
}



#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPythonHelperModule, PythonHelper)