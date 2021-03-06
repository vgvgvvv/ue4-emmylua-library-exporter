// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "EmmyLuaLibraryExporter.h"



#include "LevelEditor.h"
#include "MultiBoxBuilder.h"

#define LOCTEXT_NAMESPACE "FEmmyLuaLibraryExporterModule"



void FEmmyLuaLibraryExporterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FEmmyLuaLibraryExporterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmmyLuaLibraryExporterModule, EmmyLuaLibraryExporter)