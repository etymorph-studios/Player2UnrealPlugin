/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "Rest/P2RestBase.h"
#include "Core/P2Schemas.h"
#include "P2RestWebAPI.generated.h"


/// Contains the data required for Web API access.
UCLASS(Category = "Player2", BlueprintType)
class PLAYER2_API UP2RequestClientP2Key : public UP2Request
{
	GENERATED_BODY()

public:
	void OnRequestComplete(EP2SResponseCode Code, const FString& Content) override;


	UPROPERTY(BlueprintReadOnly) FP2SchemaClientP2Key P2Key;

	UPROPERTY(BlueprintReadOnly) EP2AuthentMode AuthentMode { EP2AuthentMode::Undefined };

};

/// Contains the data required for token generation.
UCLASS(Category = "Player2", BlueprintType)
class PLAYER2_API UP2RequestLoginDevice : public UP2Request
{
	GENERATED_BODY()

public:
	void OnRequestComplete(EP2SResponseCode Code, const FString& Content) override;

	UPROPERTY(BlueprintReadOnly) FP2SchemaAuthFlow AuthFlowData;
};

/// Proxy that contains all the interfaces for web authorization and checks.
UCLASS()
class PLAYER2_API UP2RestWeb : public UP2RestBase
{
	GENERATED_BODY()

public:

	/// Attempts to reach out to the machine Player2 process for the game client ID.
	UFUNCTION() UP2RequestClientP2Key* RequestLocalClientP2Key();

	/// Checks if the current cache key is still valid with the Player2 service.
	UFUNCTION() UP2Request* RequestCheckCacheP2KeyValid();

	/// Acts as a bridge to convert the key to the required type. Call this if the Check returns true. 
	UFUNCTION() UP2RequestClientP2Key* MocRequestClientP2KeyFromCache();

	/// Caller must also be responsible for polling for hook calls from the API.
	UFUNCTION() UP2RequestLoginDevice* RequestLoginDevice();

	/// Checks for a confirmation from the cloud using the device code. User must click approve on web-browser for success. 
	UFUNCTION() UP2RequestClientP2Key* RequestDeviceToken(const FString& DeviceCode);

	virtual FName GetEndPointType() const override;

};
