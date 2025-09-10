/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Rest/P2Login.h"
#include "Rest/P2RestWebAPI.h"
#include "Core/P2Subsystem.h"
#include "Core/P2Logger.h"
#include "Core/P2Cache.h"

#include "Kismet/KismetSystemLibrary.h"










EP2AuthentMode UP2Login::GetPlayer2AuthenticationMode() const
{
	//EP2AuthentMode Mode = EP2AuthentMode::Undefined;
	if (ClientKeyObj != nullptr)
	{
		return ClientKeyObj->AuthentMode;
	}
	return EP2AuthentMode::Undefined;
}

FString UP2Login::GetClientP2Key() const
{
	if (ClientKeyObj != nullptr)
	{
		return ClientKeyObj->P2Key.p2Key;
	}
	return TEXT("");
}

bool UP2Login::IsCurrentlyRequestingClientKeys() const
{
	bool RequestingClientKey = (ClientKeyObj != nullptr && ClientKeyObj->IsCurrentlyRequesting());
	bool RequestingClientDevice = (ClientDeviceObj != nullptr && ClientDeviceObj->IsCurrentlyRequesting());
	return RequestingClientKey || RequestingClientDevice;
}

bool UP2Login::TryLocallyLoggingIn()
{
	if (IsCurrentlyRequestingClientKeys()) { return false; }

	UP2RestWeb* RestWeb = P2Subsystem->GetRestEndpointRaw<UP2RestWeb>();
	check(RestWeb != nullptr);
	ClientKeyObj = RestWeb->RequestLocalClientP2Key();
	check(ClientKeyObj != nullptr);

	if (ClientKeyObj->IsCurrentlyRequesting())
	{
		ClientKeyObj->OnP2RequestComplete.AddDynamic(this, &UP2Login::OnLocalRequestClientP2Key);
		return true;
	}
	else
	{
		P2_LOG_E(TEXT("services couldn't create local request to client."));
		OnP2LoginAuthenticated.ExecuteIfBound(this, false);
		OnP2LoginStatusChange.Broadcast(this, EP2LoginStatusChange::Unauthenticated);
		return false;
	}
}

void UP2Login::InitStartup(UObject* Caller, FOnP2LoginAuthenticated AuthenticationEvent)
{
	P2_LOG_I(TEXT("Initializing login services"));

	check(Caller != nullptr && Caller->GetWorld()!=nullptr);
	P2Subsystem = Caller->GetWorld()->GetGameInstance()->GetSubsystem<UP2Subsystem>();
	check(P2Subsystem != nullptr);

	OnP2LoginAuthenticated = MoveTemp(AuthenticationEvent);

	UP2RestWeb* RestWeb = P2Subsystem->GetRestEndpointRaw<UP2RestWeb>();
	check(RestWeb != nullptr);
	ClientKeyObj = RestWeb->RequestLocalClientP2Key();
	check(ClientKeyObj != nullptr);

	if (ClientKeyObj->IsCurrentlyRequesting())
	{
		ClientKeyObj->OnP2RequestComplete.AddDynamic(this, &UP2Login::OnInitRequestLocalClientP2Key);
	}
	else
	{
		P2_LOG_E(TEXT("services couldn't create local request to client."));
		OnP2LoginAuthenticated.ExecuteIfBound(this, false);
		OnP2LoginStatusChange.Broadcast(this, EP2LoginStatusChange::Unauthenticated);
	}


}


void UP2Login::OnInitRequestLocalClientP2Key(UP2Request* Response)
{
	// Safety check. we should only be doing a single request at a time.
	check(Response == ClientKeyObj);
	
	ClientKeyObj = Cast<UP2RequestClientP2Key>(Response);
	if (ClientKeyObj != nullptr)
	{
		if (ClientKeyObj->ResponseCode == EP2SResponseCode::Success)
		{
			P2_LOG_I(TEXT("login services successfully login locally!"));
			OnP2LoginAuthenticated.ExecuteIfBound(this, true);
			OnP2LoginStatusChange.Broadcast(this, EP2LoginStatusChange::Authenticated);
			return;
		}

		UP2RestWeb* RestWeb = P2Subsystem->GetRestEndpointRaw<UP2RestWeb>();
		check(RestWeb != nullptr);
		ClientKeyObj = nullptr;
		UP2Request* CheckRequest = RestWeb->RequestCheckCacheP2KeyValid();

		if (CheckRequest!= nullptr && CheckRequest->IsCurrentlyRequesting())
		{
			CheckRequest->OnP2RequestComplete.AddDynamic(this, &UP2Login::OnInitRequestCloudClientP2Key);
		}
		else
		{
			P2_LOG_E(TEXT("services couldn't create cloud request to client."));
			OnP2LoginAuthenticated.ExecuteIfBound(this, false);
			OnP2LoginStatusChange.Broadcast(this, EP2LoginStatusChange::Unauthenticated);
		}
	}
}

void UP2Login::OnInitRequestCloudClientP2Key(UP2Request* Response)
{
	// Safety check. we should only be doing a single request at a time.
	check(Response != nullptr);
	EP2LoginStatusChange status = EP2LoginStatusChange::Unauthenticated;

	if (Response != nullptr && Response->ResponseCode == EP2SResponseCode::Success)
	{
		UP2RestWeb* RestWeb = P2Subsystem->GetRestEndpointRaw<UP2RestWeb>();
		check(RestWeb != nullptr);
		ClientKeyObj = RestWeb->MocRequestClientP2KeyFromCache();

		status = EP2LoginStatusChange::Authenticated;
		P2_LOG_I(TEXT("login services successfully login to the cloud!"));
		OnP2LoginAuthenticated.ExecuteIfBound(this, true);
	}
	else
	{
		P2_LOG_I(TEXT("login services failed to login."));
		OnP2LoginAuthenticated.ExecuteIfBound(this, false);
	}


	OnP2LoginStatusChange.Broadcast(this, status);
}

void UP2Login::OnLocalRequestClientP2Key(UP2Request* Response)
{
	check(Response != nullptr);
	
	P2_LOG_I(TEXT("login services: local request was successful: %i"), (Response->ResponseCode == EP2SResponseCode::Success) ? "true" : "false");

	EP2LoginStatusChange Status = (Response->ResponseCode == EP2SResponseCode::Success) ? EP2LoginStatusChange::Authenticated : EP2LoginStatusChange::Unauthenticated;
	OnP2LoginAuthenticated.ExecuteIfBound(this, Response->ResponseCode == EP2SResponseCode::Success);
	OnP2LoginStatusChange.Broadcast(this, Status);
}


bool UP2Login::IsWaitingForUserToAuth() const
{
	return AuthPoolingInfo.TimerHandle.IsValid();
}

void UP2Login::StopPoolingForAuthDevice()
{
	bool IsRunning = IsWaitingForUserToAuth();
	StopPoolingForAuthDeviceInternal();
	if (IsRunning)
	{
		OnP2LoginAuthenticated.ExecuteIfBound(this, false);
		OnP2LoginStatusChange.Broadcast(this, EP2LoginStatusChange::Unauthenticated);
	}
}

void UP2Login::StopPoolingForAuthDeviceInternal()
{
	if (AuthPoolingInfo.TimerHandle.IsValid())
	{
		P2Subsystem->GetGameInstance()->GetTimerManager().ClearTimer(AuthPoolingInfo.TimerHandle);
	}
	if (AuthPoolingInfo.TargetRequester != nullptr)
	{
		AuthPoolingInfo.TargetRequester->OnP2RequestComplete.RemoveAll(this);
	}

	AuthPoolingInfo = FPoolingInfo();
}

bool UP2Login::TryStartUpLogin()
{
	if (IsCurrentlyRequestingClientKeys()) { return false; }

	UP2RestWeb* RestWeb = P2Subsystem->GetRestEndpointRaw<UP2RestWeb>();
	check(RestWeb != nullptr);
	ClientDeviceObj = RestWeb->RequestLoginDevice();
	
	if (ClientDeviceObj->IsCurrentlyRequesting())
	{
		ClientDeviceObj->OnP2RequestComplete.AddDynamic(this, &UP2Login::OnStartUpLoginComplete);
		return true;
	}
	else
	{
		P2_LOG_E(TEXT("services couldn't start up the login process."));
		return false;
	}
}

void UP2Login::OnStartUpLoginComplete(UP2Request* Response)
{
	check(Response == ClientDeviceObj);
	ClientDeviceObj = Cast<UP2RequestLoginDevice>(Response);

	if (ClientDeviceObj != nullptr && ClientDeviceObj->ResponseCode == EP2SResponseCode::Success && !ClientDeviceObj->AuthFlowData.verificationUriComplete.IsEmpty())
	{
		StopPoolingForAuthDeviceInternal();
		AuthPoolingInfo.AuthFlow = ClientDeviceObj->AuthFlowData;
		AuthPoolingInfo.StartTime = FDateTime::UtcNow().ToUnixTimestampDecimal();

		UKismetSystemLibrary::LaunchURL(AuthPoolingInfo.AuthFlow.verificationUriComplete);
		
		FTimerDelegate pollDelegate;
		pollDelegate.BindUObject(this, &UP2Login::OnPollForDeviceNew);
		float IntervalTime = FMath::Max(1.0f, AuthPoolingInfo.AuthFlow.interval);
		P2Subsystem->GetGameInstance()->GetTimerManager().SetTimer(AuthPoolingInfo.TimerHandle, pollDelegate, IntervalTime, false);
		
		P2_LOG_I(TEXT("Starting up login request was accepted. Starting next steps."));
		P2_LOG_V(TEXT("AuthData: {%s}"),*P2::JsonToString(AuthPoolingInfo.AuthFlow.ToJson()));
		OnP2LoginStatusChange.Broadcast(this, EP2LoginStatusChange::UserLogin);
	}
	else
	{
		P2_LOG_W(TEXT("Starting up login request was denied. Dropping next steps.[%s]"), (ClientDeviceObj ? *StaticEnum<EP2SResponseCode>()->GetNameStringByValue((int64)ClientDeviceObj->ResponseCode)  :TEXT("")));
		OnP2LoginAuthenticated.ExecuteIfBound(this, false);
		OnP2LoginStatusChange.Broadcast(this, EP2LoginStatusChange::Unauthenticated);
	}

}

void UP2Login::OnPollForDeviceNew()
{
	checkf(AuthPoolingInfo.TargetRequester == nullptr, TEXT("Should not poll a new request as we are already waiting on the last poll response."));


	if (AuthPoolingInfo.StartTime + AuthPoolingInfo.AuthFlow.expiresIn < FDateTime::UtcNow().ToUnixTimestampDecimal())
	{
		StopPoolingForAuthDeviceInternal();
		OnP2LoginAuthenticated.ExecuteIfBound(this, false);
		OnP2LoginStatusChange.Broadcast(this, EP2LoginStatusChange::Timeout);
		return;
	}

	if(AuthPoolingInfo.TargetRequester != nullptr) { return; }


	UP2RestWeb* RestWeb = P2Subsystem->GetRestEndpointRaw<UP2RestWeb>();
	check(RestWeb != nullptr);
	AuthPoolingInfo.TargetRequester = RestWeb->RequestDeviceToken(AuthPoolingInfo.AuthFlow.deviceCode);

	if (AuthPoolingInfo.TargetRequester->IsCurrentlyRequesting())
	{
		P2_LOG_V(TEXT("Pooling for new token"));
		AuthPoolingInfo.TargetRequester->OnP2RequestComplete.AddDynamic(this, &UP2Login::OnCompletePollDeviceRequest);
	}
	else
	{
		AuthPoolingInfo.TargetRequester = nullptr;
		P2_LOG_V(TEXT("couldn't Pooling for new token"));
		FTimerDelegate pollDelegate;
		pollDelegate.BindUObject(this, &UP2Login::OnPollForDeviceNew);
		float IntervalTime = FMath::Max(1.0f, AuthPoolingInfo.AuthFlow.interval);
		P2Subsystem->GetGameInstance()->GetTimerManager().SetTimer(AuthPoolingInfo.TimerHandle, pollDelegate, IntervalTime, false);
	}

}

void UP2Login::OnCompletePollDeviceRequest(UP2Request* Response)
{
	checkf(Response == AuthPoolingInfo.TargetRequester, TEXT("Should only be handling 1 response at a time with polling devices."));
	if (Response != AuthPoolingInfo.TargetRequester) { return; }

	if (AuthPoolingInfo.TargetRequester->ResponseCode == EP2SResponseCode::Success && !AuthPoolingInfo.TargetRequester->P2Key.p2Key.IsEmpty())
	{
		ClientKeyObj = AuthPoolingInfo.TargetRequester;
		P2_LOG_V(TEXT("User confirmed login, id: %s"), *ClientKeyObj->P2Key.p2Key);
		StopPoolingForAuthDeviceInternal();
		UP2Cache::SetBearerToken(*ClientKeyObj->P2Key.p2Key);
		OnP2LoginAuthenticated.ExecuteIfBound(this, true);
		OnP2LoginStatusChange.Broadcast(this, EP2LoginStatusChange::Authenticated);
	}
	else
	{
		AuthPoolingInfo.TargetRequester = nullptr;
		FTimerDelegate pollDelegate;
		pollDelegate.BindUObject(this, &UP2Login::OnPollForDeviceNew);
		float IntervalTime = FMath::Max(1.0f, AuthPoolingInfo.AuthFlow.interval);
		P2Subsystem->GetGameInstance()->GetTimerManager().SetTimer(AuthPoolingInfo.TimerHandle, pollDelegate, IntervalTime, false);
	}
}

