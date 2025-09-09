/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "Core/P2GenericInfo.h"
#include "Core/P2Schemas.h"
#include "CoreMinimal.h"
#include "P2Login.generated.h"


class UP2RestBase;
class UP2Request;
class UP2RequestClientP2Key;
class UP2RequestLoginDevice;
class UP2Subsystem;

DECLARE_DELEGATE_OneParam(FOnP2LoginStatusChange, class UP2Login*);

UCLASS(Category = "Player2")
class PLAYER2_API UP2Login : public UObject
{
	GENERATED_BODY()

public:
	
	
	void InitStartup();

	/// Checks if we are currently requesting client keys.
	UFUNCTION(BlueprintPure) bool IsCurrentlyRequestingClientKeys() const;

	/// Checks if the client has a local process of Player2 running in the background.
	UFUNCTION(BlueprintPure) bool IsPlayer2RunningLocally() const;

	/// @return the player2 key if it exist, otherwise the result will be empty.
	UFUNCTION(BlueprintPure) FString GetClientP2Key() const;

	///@return true if a request was sent.
	UFUNCTION(BlueprintCallable) bool TryLocallyLoggingIn();


	UFUNCTION(BlueprintCallable) bool TryStartUpLogin();

	/// Checks if we are waiting for the user to sign in from the 3rd party link.
	UFUNCTION(BlueprintPure) bool IsWaitingForUserToAuth() const;

	/// Cancels the current authority flow.
	UFUNCTION(BlueprintCallable) void StopPoolingForAuthDevice();


protected:
	friend UP2Subsystem;
	FOnP2LoginStatusChange OnP2LoginStatusChange;
	UP2Subsystem* P2Subsystem{ nullptr };

private:
	UPROPERTY(Transient) UP2RequestClientP2Key* ClientKeyObj { nullptr };
	UPROPERTY(Transient) UP2RequestLoginDevice* ClientDeviceObj { nullptr };
	


	UFUNCTION() void OnInitRequestLocalClientP2Key(UP2Request* Response);
	UFUNCTION() void OnInitRequestCloudClientP2Key(UP2Request* Response);


	UFUNCTION() void OnLocalRequestClientP2Key(UP2Request* Response);
	UFUNCTION() void OnStartUpLoginComplete(UP2Request* Response);




	struct FPoolingInfo {
		FTimerHandle TimerHandle;
		FP2SchemaAuthFlow AuthFlow;
		double StartTime{ -1 };
	};
	FPoolingInfo AuthPoolingInfo;

	UFUNCTION() void OnPollForDeviceNew();

};



