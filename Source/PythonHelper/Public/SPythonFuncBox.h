// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"


class SPythonFuncBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPythonFuncBox)
	{}


	SLATE_ARGUMENT(FString, FullPath)
	SLATE_ARGUMENT(FString, FunctionName)
	SLATE_ARGUMENT(FString, Alias)
	SLATE_END_ARGS()

private:


public:

	void Construct(const FArguments& Args);

	FReply OnExecFunction();


private:
	FString FullPath;
	// include "()"
	FString FunctionName;
	//FString Alias;

};
