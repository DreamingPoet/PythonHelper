// Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PythonHelperStyle.h"

class FPythonHelperCommands : public TCommands<FPythonHelperCommands>
{
public:

	FPythonHelperCommands()
		: TCommands<FPythonHelperCommands>(TEXT("PythonHelper"), NSLOCTEXT("Contexts", "PythonHelper", "PythonHelper Plugin"), NAME_None, FPythonHelperStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};