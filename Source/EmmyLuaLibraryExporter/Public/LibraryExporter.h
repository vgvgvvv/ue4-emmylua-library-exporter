#pragma once
#include "JsonObject.h"

#include "LibraryExporter.generated.h"

USTRUCT()
struct FFunctionInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FString> ParamTypes;

	UPROPERTY()
	FString ReturnType;
};

USTRUCT()
struct FFunctionInfoGroup
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	TArray<FFunctionInfo> Group;

};


class EMMYLUALIBRARYEXPORTER_API FLibraryExporter
{
public:
	static TSharedPtr<FJsonObject> ExportJson();

private:

	static void ExportClassJson(TSharedPtr<FJsonObject> Json);

	static void ExportEnumJson(TSharedPtr<FJsonObject> Json);
	
	static void ExportClassPropertiesJson(UStruct* Class, TSharedPtr<FJsonObject> ClassJson);

	static void ExportClassFunctionsJson(UStruct* Class, TSharedPtr<FJsonObject> ClassJson);

};
