// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#include "PythonHelperCommands.h"

#define LOCTEXT_NAMESPACE "FPythonHelperModule"

void FPythonHelperCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "PythonHelper", "Bring up PythonHelper window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
