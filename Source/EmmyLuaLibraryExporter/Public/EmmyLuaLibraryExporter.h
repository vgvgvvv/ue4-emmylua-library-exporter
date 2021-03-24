// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Framework/Commands/Commands.h"
#include "Modules/ModuleManager.h"

class FMenuBuilder;

class FEmmyLuaLibraryExporterModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
