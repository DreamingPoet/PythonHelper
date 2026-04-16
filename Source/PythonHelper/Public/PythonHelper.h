// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class FBrowserElement;
class SPythonFileBrowser;
class SDockTab;
class SCheckBox;


DECLARE_LOG_CATEGORY_EXTERN(LogPythonHelper, Log, All)

class FPythonHelperModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	//~ Get Project script directory
	static FString GetPythonDirectory()
	{
		return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "Python";
	}

	/** for import module， in case of same file name in different directory */
	static FString GetFilePathCharater(const FString Path);

	/** for new file or folder name, check if it is valid for python */
	static bool IsFileOrFoldNameValid(FString Name);

	static FString PluginDirectory;
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	void GenerateFunctionButtons(TSharedPtr<FBrowserElement> Item);

	TSharedPtr<SVerticalBox> FunctionContainer;

	TSharedPtr<SPythonFileBrowser> PythonFileBrowser;

	TSharedPtr<STextBlock> PythonFilenameText;

	void OnAutoRefreshPythonFile(ECheckBoxState NewState);
	bool bAutoRefresh = true;

	TSharedPtr<FBrowserElement> CurrentItem;

	void GenerateFunctions(const FString Code, const FString Path);

	FTimerHandle TimerHander;
	void TimerCall(bool bForceEndChange = false);

	void OnTabClosed(TSharedRef<SDockTab> Tab);

	void CheckDependencies();

	void OnPythonInitialized();
};
