/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "UObject/NoExportTypes.h"
#include "P2GenericInfo.generated.h"


/// Placeholder. Used with blueprints, to give expressive results.
UENUM(BlueprintType, Category = "Player2")
enum class EP2ResultSwitch : uint8
{
	OnSuccess,
	OnFailure
};

/// HTTP request call types.
UENUM(BlueprintType, Category = "Player2")
enum class EP2RequestMethod : uint8
{
	Get,
	Post,
	Put,
	Delete,
	Patch,
};


/// represents the response code gathered from the HTTP request.
UENUM(BlueprintType, Category = "Player2")
enum class EP2SResponseCode : uint8 {
	Success,
	InvalidArgument,
	NotAuthenticated,
	InsufficientCredits,
	UnknownPage,
	AlreadyStarted,
	ToManyRequests,
	InternalServerError,
	Undefined
};



namespace P2
{
	FString JsonToString(const TSharedPtr<FJsonObject>& JsonObj);
}