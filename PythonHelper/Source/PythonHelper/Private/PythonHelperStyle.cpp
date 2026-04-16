// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#include "PythonHelperStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateTypes.h"

TSharedPtr< FSlateStyleSet > FPythonHelperStyle::StyleInstance = NULL;

void FPythonHelperStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FPythonHelperStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FPythonHelperStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("PythonHelperStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon32x32(32.0f, 32.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);

TSharedRef< FSlateStyleSet > FPythonHelperStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("PythonHelperStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("PythonHelper")->GetBaseDir() / TEXT("Resources"));

	Style->Set("PythonHelper.PythonIcon16", new IMAGE_BRUSH(TEXT("PythonIcon_16x"), Icon16x16));
	Style->Set("PythonHelper.FolderIcon", new IMAGE_BRUSH(TEXT("PythonFolder_32x"), Icon32x32));
	Style->Set("PythonHelper.FileIcon", new IMAGE_BRUSH(TEXT("PythonFile_32x"), Icon32x32));
	Style->Set("PythonHelper.Refresh", new IMAGE_BRUSH(TEXT("Refresh_32x"), Icon16x16));
	Style->Set("PythonHelper.White1x", new IMAGE_BRUSH(TEXT("White_1x"), FVector2D(1,2)));

// 
// 	Style->Set("PythonHelper.Splitter", FSplitterStyle()
// 		.SetHandleNormalBrush(FSlateColorBrush(FLinearColor(FColor(0, 0, 0))))
// 		.SetHandleHighlightBrush(FSlateColorBrush(FLinearColor(FColor(128, 128, 128))))
// 	);

	// Open Python folder button
	FButtonStyle OpenPythonFolder;
	Style->Set("PythonHelper.OpenPythonFolder", new IMAGE_BRUSH(TEXT("OpenPythonFolder_64x"), Icon64x64));
	OpenPythonFolder.Normal = *Style->GetBrush("PythonHelper.OpenPythonFolder");
	OpenPythonFolder.Normal.DrawAs = ESlateBrushDrawType::Image;
	OpenPythonFolder.Normal.ImageSize = Icon40x40;
	OpenPythonFolder.Hovered = *Style->GetBrush("PythonHelper.OpenPythonFolder");
	OpenPythonFolder.Hovered.DrawAs = ESlateBrushDrawType::Image;
	OpenPythonFolder.Hovered.ImageSize = Icon40x40;
	OpenPythonFolder.Pressed = *Style->GetBrush("PythonHelper.OpenPythonFolder");
	OpenPythonFolder.Pressed.DrawAs = ESlateBrushDrawType::Image;
	OpenPythonFolder.Pressed.ImageSize = Icon40x40;
	OpenPythonFolder.Pressed.TintColor = FLinearColor::Gray;
	Style->Set("PythonHelper.Button.OpenPythonFolder", OpenPythonFolder);

	// Execute script file button
	FButtonStyle RunScript;
	Style->Set("PythonHelper.RunScript", new IMAGE_BRUSH("RunScript_16x", Icon16x16));
	RunScript.Normal = *Style->GetBrush("PythonHelper.RunScript");
	RunScript.Normal.DrawAs = ESlateBrushDrawType::Image;
	RunScript.Hovered = *Style->GetBrush("PythonHelper.RunScript");
	RunScript.Hovered.DrawAs = ESlateBrushDrawType::Image;
	RunScript.Pressed = *Style->GetBrush("PythonHelper.RunScript");
	RunScript.Pressed.DrawAs = ESlateBrushDrawType::Image;
	RunScript.Pressed.TintColor = FLinearColor::Gray;
	Style->Set("PythonHelper.Button.RunScript", RunScript);

	// Execute function button
	FButtonStyle RunFunction;
	Style->Set("PythonHelper.FuncButton", new IMAGE_BRUSH(TEXT("Button_32x"), Icon64x64));
	RunFunction.Normal = *Style->GetBrush("PythonHelper.FuncButton");
	RunFunction.Normal.DrawAs = ESlateBrushDrawType::Box;
	RunFunction.Normal.Margin = FMargin(0.25);
	RunFunction.Normal.ImageSize = Icon32x32;
	RunFunction.Hovered = *Style->GetBrush("PythonHelper.FuncButton");
	RunFunction.Hovered.DrawAs = ESlateBrushDrawType::Box;
	RunFunction.Hovered.Margin = FMargin(0.25);
	RunFunction.Hovered.ImageSize = Icon32x32;
	RunFunction.Pressed = *Style->GetBrush("PythonHelper.FuncButton");
	RunFunction.Pressed.DrawAs = ESlateBrushDrawType::Box;
	RunFunction.Pressed.Margin = FMargin(0.25);
	RunFunction.Pressed.ImageSize = Icon32x32;
	RunFunction.Pressed.TintColor = FLinearColor::Gray;
	Style->Set("PythonHelper.Button.RunFunction", RunFunction);

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FPythonHelperStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FPythonHelperStyle::Get()
{
	return *StyleInstance;
}
