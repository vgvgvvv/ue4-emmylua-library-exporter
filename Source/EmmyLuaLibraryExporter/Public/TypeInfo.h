#pragma once

#include "CoreMinimal.h"

#include "PropertyPath.h"

#include "TypeInfo.generated.h"

USTRUCT()
struct FFunctionMetaInfo
{
	GENERATED_BODY()
public:
};

USTRUCT()
struct FPropertyMetaInfo
{
	GENERATED_BODY()
public:
	FString PropertyName;
};


USTRUCT()
struct FClassMetaInfo
{
	GENERATED_BODY()
public:
	TArray<FPropertyMetaInfo> Properties;
	TArray<FFunctionMetaInfo> Functions;
};

USTRUCT()
struct FEnumMetaInfo
{
	GENERATED_BODY()
public:
	
};






