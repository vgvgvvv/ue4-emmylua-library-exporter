#pragma once
#include "JsonObject.h"

class EMMYLUALIBRARYEXPORTER_API FLibraryExporter
{
public:
	static TSharedPtr<FJsonObject> ExportJson();

private:

	static void ExportClassJson(TSharedPtr<FJsonObject> Json);

	static void ExportEnumJson(TSharedPtr<FJsonObject> Json);
	
	static void ExportClassPropertiesJson(UStruct* Class, TSharedPtr<FJsonObject> ClassJson);

	static void ExportClassFunctionsJson(UStruct* Class, TSharedPtr<FJsonObject> ClassJson);

	static void ExportSingleFunction(UFunction* Function, TSharedPtr<FJsonObject> FunctionJson);
};
