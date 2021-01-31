#include "LibraryExporter.h"



#include "Casts.h"
#include "Class.h"
#include "UnrealType.h"
#include "UObjectHash.h"

TSharedPtr<FJsonObject> FLibraryExporter::ExportJson()
{
	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	
	ExportClassJson(Result);
	ExportEnumJson(Result);

	return Result;
}


void FLibraryExporter::ExportEnumJson(TSharedPtr<FJsonObject> Json)
{
	TArray<UObject*> Enums;
	GetObjectsOfClass(UEnum::StaticClass(), Enums);

	auto EnumsJsonObject = MakeShared<FJsonObject>();
	Json->SetObjectField("Enumes", EnumsJsonObject);

	for(auto Object : Enums)
	{
		auto Enum = Cast<UEnum>(Object);
		auto EnumObject = MakeShared<FJsonObject>();
		EnumsJsonObject->SetObjectField(Enum->GetName(), EnumObject);
		
		auto EnumNum = Enum->NumEnums();
		for(int i = 0; i < EnumNum; i ++)
		{
			auto Name = Enum->GetNameByIndex(i);
			auto Value = Enum->GetValueByIndex(i);
			EnumObject->SetNumberField(Name.ToString(), Value);
		}
	}
	
}

void FLibraryExporter::ExportClassJson(TSharedPtr<FJsonObject> Json)
{
	TArray<UObject*> Structs;
	GetObjectsOfClass(UStruct::StaticClass(), Structs);

	auto ClassGroup = MakeShared<FJsonObject>();
	Json->SetObjectField("Classes", ClassGroup);
	
	for(auto Object : Structs)
	{
		auto Class = Cast<UStruct>(Object);
		auto ClassObject = MakeShared<FJsonObject>();
		ClassGroup->SetObjectField(Class->GetName(), ClassObject);

		ExportClassPropertiesJson(Class, ClassObject);
		ExportClassFunctionsJson(Class, ClassObject);
	}
}

void FLibraryExporter::ExportClassPropertiesJson(UStruct* Class, TSharedPtr<FJsonObject> ClassJson)
{
	auto PropertiesGroupJson = MakeShared<FJsonObject>();
	ClassJson->SetObjectField("Properties", PropertiesGroupJson);
	for (TFieldIterator<UProperty> Iterator(Class); Iterator; ++Iterator)
	{
		auto Name = Iterator->GetNameCPP();
		auto PropertyClass = Iterator->GetCPPType();
		PropertiesGroupJson->SetStringField(Name, PropertyClass);
	}
}


void FLibraryExporter::ExportClassFunctionsJson(UStruct* Class, TSharedPtr<FJsonObject> ClassJson)
{
	auto FunctionsGroupJson = MakeShared<FJsonObject>();
	ClassJson->SetObjectField("Functions", FunctionsGroupJson);
	for (TFieldIterator<UFunction> Iterator(Class); Iterator; ++Iterator)
	{
		auto Name = Iterator->GetName();
		auto FunctionObjectJson = MakeShared<FJsonObject>();
		FunctionsGroupJson->SetObjectField(Name, FunctionObjectJson);

		ExportSingleFunction(*Iterator, FunctionObjectJson);
	}
}


void FLibraryExporter::ExportSingleFunction(UFunction* Function, TSharedPtr<FJsonObject> FunctionJson)
{
	
}
