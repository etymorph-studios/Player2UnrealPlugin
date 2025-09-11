/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "UObject/NoExportTypes.h"
#include "P2GenericInfo.generated.h"

namespace P2
{
	void PluginVersion(int32& Major, int32& Minor, int32& Patch);
	FString PluginVersion();

	FString JsonToString(const TSharedPtr<FJsonObject>& JsonObj);
}


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

/// The state in which player2 can function. Local instance will take precedence, otherwise alternatives will be attempted.
UENUM(BlueprintType, Category = "Player2")
enum class EP2AuthentMode : uint8
{
	/// Local instance running on machine.
	Local,
	/// The API key received from the Player2 redirect page.
	Bearer,
	/// Not defined
	ApiToken,
	Undefined
};


