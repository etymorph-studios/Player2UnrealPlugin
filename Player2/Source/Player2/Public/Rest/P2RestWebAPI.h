/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "Rest/P2RestBase.h"
#include "Core/P2Schemas.h"
#include "P2RestWebAPI.generated.h"


UCLASS(Category = "Player2", BlueprintType)
class PLAYER2_API UP2RequestClientP2Key : public UP2Request
{
	GENERATED_BODY()

public:
	void OnRequestComplete(EP2SResponseCode Code, const FString& Content) override;


	UPROPERTY(BlueprintReadOnly) FP2SchemaClientP2Key P2Key;

	UPROPERTY(BlueprintReadOnly) bool LocalMachineCallFlag{ false };

};


UCLASS(Category = "Player2", BlueprintType)
class PLAYER2_API UP2RequestLoginDevice : public UP2Request
{
	GENERATED_BODY()

public:
	void OnRequestComplete(EP2SResponseCode Code, const FString& Content) override;


	UPROPERTY(BlueprintReadOnly) FP2SchemaAuthFlow AuthFlowData;

};




UCLASS()
class PLAYER2_API UP2RestWeb : public UP2RestBase
{
	GENERATED_BODY()

public:

	/// Attempts to reach out to the machine Player2 process for the game client ID.
	UFUNCTION(BlueprintCallable) UP2RequestClientP2Key* RequestLocalClientP2Key();

	UFUNCTION(BlueprintCallable) UP2RequestClientP2Key* RequestCloudClientP2Key();

	/// Caller must also be responsible for polling for hook calls from the API.
	UFUNCTION(BlueprintCallable) UP2RequestLoginDevice* RequestLoginDevice();

	virtual FName GetEndPointType() const override;

};
