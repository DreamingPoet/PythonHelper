// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#include "SPythonFuncBox.h"
#include "PythonHelperStyle.h"
#include "PythonHelper.h"
#include "Widgets/Layout/SScrollBox.h"
#include "IPythonScriptPlugin.h"



void SPythonFuncBox::Construct(const FArguments& Args)
{
	FullPath = Args._FullPath;
	FunctionName = Args._FunctionName;
	//Alias = Args._Alias;
	FString ShowName = Args._Alias == "" ? FunctionName : Args._Alias;
	ChildSlot[
		SNew(SButton)
			.OnClicked(this, &SPythonFuncBox::OnExecFunction)
			.ButtonStyle(&FPythonHelperStyle::Get().GetWidgetStyle<FButtonStyle>("PythonHelper.Button.RunFunction"))
			.ContentPadding(FMargin(10))
			[
				SNew(STextBlock).Text(FText::FromString("Call  " + ShowName)).ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
			]
	];



}

FReply SPythonFuncBox::OnExecFunction()
{
	FString script = FPythonHelperModule::GetFilePathCharater(FullPath) +"."+ FunctionName;

	IPythonScriptPlugin::Get()->ExecPythonCommand(*script);

	return FReply::Handled();

}
