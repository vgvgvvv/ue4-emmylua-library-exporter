#include "LibraryExporter.h"



#include "Templates/Casts.h"
#include "UObject/Class.h"
#include "Misc/FileHelper.h"
#include "JsonObjectConverter.h"
#include "Misc/Paths.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectHash.h"

TSharedPtr<FJsonObject> FLibraryExporter::ExportJson()
{
	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	
	ExportClassJson(Result);
	ExportEnumJson(Result);

	return Result;
}


FString FLibraryExporter::ExportJsonString()
{
	using FJsonObjectPtr = TSharedPtr<FJsonObject>;
	using FJsonWriterRef = TSharedRef<TJsonWriter<>>;
	using FJsonReaderRef = TSharedRef<TJsonReader<>>;
	
	TSharedPtr<FJsonObject> libraryJson = ExportJson();
	FString txtLibrary;
	FJsonWriterRef Writer = TJsonWriterFactory<>::Create(&txtLibrary);
	FJsonSerializer::Serialize(libraryJson.ToSharedRef(), Writer);

	return txtLibrary;
}


void FLibraryExporter::ExportLibrary(const FString& ExePath, const FString& OutputPath)
{
	FString JsonPath = FPaths::ProjectSavedDir() + "emmy_lua_library.json";
	FString JsonTxt = ExportJsonString();
	FFileHelper::SaveStringToFile(JsonTxt, *JsonPath);
	FString Params = FString::Printf(TEXT("in=%s out=%s"), *JsonPath, *OutputPath);
	FPlatformProcess::CreateProc(*ExePath, *Params, true, false, false, nullptr, 0, nullptr, nullptr);
}

void FLibraryExporter::ExportLibrary(const FString& OutputPath)
{
	FString exePath = FPaths::ProjectPluginsDir() + TEXT("EmmyLuaLibraryExporter/LibraryExporter/Binaries/EmmyLuaLibraryExporter.exe");
	ExportLibrary(exePath, OutputPath);
}


void FLibraryExporter::ExportLibrary()
{
	FString savePath = FPaths::ProjectSavedDir() + TEXT("LuaHint");
	ExportLibrary(savePath);
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
		auto SuperClass = Class->GetSuperStruct();
		auto ClassObject = MakeShared<FJsonObject>();
		ClassGroup->SetObjectField(Class->GetName(), ClassObject);

		if(SuperClass)
		{
			ClassObject->SetStringField("Parent", SuperClass->GetName());
		}
		
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
		auto IsStatic = FunctionIt->HasAllFunctionFlags(EFunctionFlags::FUNC_Static);

		if(FunctionGroup == nullptr)
		{
			FunctionGroupMap.Add(FunctionName, FFunctionInfoGroup());
			FunctionGroup = FunctionGroupMap.Find(FunctionName);
		}
		
		FunctionGroup->Name = FunctionName;
		FunctionGroup->IsStatic = IsStatic;
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

		auto FunctionGroup = Pair.Value;

		GroupInfo->SetBoolField("IsStatic", FunctionGroup.IsStatic);
		
		TArray<TSharedPtr<FJsonValue>> FunctionInfos;
		for(FFunctionInfo& FunctionInfo : FunctionGroup.Group)
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

