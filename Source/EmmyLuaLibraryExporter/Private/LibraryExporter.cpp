#include "LibraryExporter.h"



#include "Casts.h"
#include "Class.h"
#include "JsonObjectConverter.h"
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
	
	
	TMap<FString, FFunctionInfoGroup> FunctionGroupMap;
	for (TFieldIterator<UFunction> FunctionIt(Class); FunctionIt; ++FunctionIt)
	{
		auto FunctionName = FunctionIt->GetName();
		auto FunctionGroup = FunctionGroupMap.Find(FunctionName);

		if(FunctionGroup == nullptr)
		{
			FunctionGroupMap.Add(FunctionName, FFunctionInfoGroup());
			FunctionGroup = FunctionGroupMap.Find(FunctionName);
		}
		
		FunctionGroup->Name = FunctionName;
		FFunctionInfo FunctionInfo;
		for(TFieldIterator<UProperty> ParamIt(*FunctionIt); ParamIt; ++ParamIt)
		{
			auto ParamName = ParamIt->GetNameCPP();
			auto ParamType = ParamIt->GetCPPType();
			if(!ParamIt->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				FunctionInfo.ParamTypes.Add(ParamName, ParamType);
			}else
			{
				FunctionInfo.ReturnType = ParamType;
			}
		}

		FunctionGroup->Group.Add(FunctionInfo);
	}

	if(FunctionGroupMap.Num() == 0)
	{
		return;
	}

	auto FunctionsGroupJson = MakeShared<FJsonObject>();
	ClassJson->SetObjectField("Functions", FunctionsGroupJson);

	for(auto& Pair : FunctionGroupMap)
	{
		auto GroupInfo = MakeShared<FJsonObject>();
		FunctionsGroupJson->SetObjectField(Pair.Key, GroupInfo);

		TArray<TSharedPtr<FJsonValue>> FunctionInfos;
		for(FFunctionInfo& FunctionInfo : Pair.Value.Group)
		{
			auto FunctionInfoObject = MakeShared<FJsonObject>();
			FunctionInfoObject->SetStringField("ReturnType", FunctionInfo.ReturnType);

			auto ParamObject = MakeShared<FJsonObject>();
			FunctionInfoObject->SetObjectField("Params", ParamObject);

			for(auto& ParamPair : FunctionInfo.ParamTypes)
			{
				ParamObject->SetStringField(ParamPair.Key, ParamPair.Value);
			}

			FunctionInfos.Add(MakeShared<FJsonValueObject>(FunctionInfoObject));
		}

		GroupInfo->SetArrayField("Overrides", FunctionInfos);
		
	}
}

