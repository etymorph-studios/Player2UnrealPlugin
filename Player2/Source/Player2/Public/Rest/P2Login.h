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

/// Events that could cause the status calls
UENUM(BlueprintType, Category = "Player2")
enum class EP2LoginStatusChange : uint8
{
	/// Routine To Start up.
	StartUp,
	/// The login was successful
	Authenticated,
	/// The login was not successful
	Unauthenticated,
	/// The user is required to login through 3rd party
	UserLogin,
	/// If the user took to long to sign in
	Timeout,
	Undefined
};

DECLARE_DELEGATE_TwoParams(FOnP2LoginAuthenticated, class UP2Login*, bool /*AuthenticatedFlag*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnP2LoginStatusChange, class UP2Login*, caller, EP2LoginStatusChange, Event);

UCLASS(Category = "Player2")
class PLAYER2_API UP2Login : public UObject
{
	GENERATED_BODY()

public:
	
	/// Attach UI to these events to showcase current login states.
	UPROPERTY(BlueprintAssignable) FOnP2LoginStatusChange OnP2LoginStatusChange;
	
	void InitStartup(UObject* Caller, FOnP2LoginAuthenticated AuthenticationEvent);

	/// Checks if we are currently requesting client keys.
	UFUNCTION(BlueprintPure) bool IsCurrentlyRequestingClientKeys() const;

	/// Checks what instance of player2 to use.
	UFUNCTION(BlueprintPure) EP2AuthentMode GetPlayer2AuthenticationMode() const;



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
	UP2Subsystem* P2Subsystem{ nullptr };
	FOnP2LoginAuthenticated OnP2LoginAuthenticated;

private:
	UPROPERTY(Transient) UP2RequestClientP2Key* ClientKeyObj { nullptr };
	UPROPERTY(Transient) UP2RequestLoginDevice* ClientDeviceObj { nullptr };
	


	UFUNCTION() void OnInitRequestLocalClientP2Key(UP2Request* Response);
	UFUNCTION() void OnInitRequestCloudClientP2Key(UP2Request* Response);


	UFUNCTION() void OnLocalRequestClientP2Key(UP2Request* Response);
	UFUNCTION() void OnStartUpLoginComplete(UP2Request* Response);


	void StopPoolingForAuthDeviceInternal();


	struct FPoolingInfo {
		FTimerHandle TimerHandle;
		FP2SchemaAuthFlow AuthFlow;
		double StartTime{ -1 };
		UP2RequestClientP2Key* TargetRequester{ nullptr };
	};
	FPoolingInfo AuthPoolingInfo;

	UFUNCTION() void OnPollForDeviceNew();
	UFUNCTION() void OnCompletePollDeviceRequest(UP2Request* Response);


};



