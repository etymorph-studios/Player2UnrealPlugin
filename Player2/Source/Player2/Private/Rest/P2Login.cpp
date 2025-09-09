/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Rest/P2Login.h"
#include "Rest/P2RestWebAPI.h"
#include "Core/P2Subsystem.h"
#include "Core/P2Logger.h"

bool UP2Login::IsPlayer2RunningLocally() const
{
	return ClientKeyObj != nullptr && ClientKeyObj->LocalMachineCallFlag;
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
		OnP2LoginStatusChange.ExecuteIfBound(this);
		return false;
	}
}

void UP2Login::InitStartup()
{
	check(P2Subsystem != nullptr);


	P2_LOG_I(TEXT("Initializing login services"));



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
		OnP2LoginStatusChange.ExecuteIfBound(this);
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
			OnP2LoginStatusChange.ExecuteIfBound(this);
			return;
		}

		UP2RestWeb* RestWeb = P2Subsystem->GetRestEndpointRaw<UP2RestWeb>();
		check(RestWeb != nullptr);
		ClientKeyObj = RestWeb->RequestCloudClientP2Key();
		check(ClientKeyObj != nullptr);

		if (ClientKeyObj->IsCurrentlyRequesting())
		{
			ClientKeyObj->OnP2RequestComplete.AddDynamic(this, &UP2Login::OnInitRequestCloudClientP2Key);
		}
		else
		{
			P2_LOG_E(TEXT("services couldn't create cloud request to client."));
			OnP2LoginStatusChange.ExecuteIfBound(this);
		}
	}
}

void UP2Login::OnInitRequestCloudClientP2Key(UP2Request* Response)
{
	// Safety check. we should only be doing a single request at a time.
	check(Response == ClientKeyObj);

	ClientKeyObj = Cast<UP2RequestClientP2Key>(Response);
	if (! ClientKeyObj->P2Key.p2Key.IsEmpty())
	{
		P2_LOG_I(TEXT("login services successfully login to the cloud!"));
	}
	else
	{
		P2_LOG_I(TEXT("login services failed to login."));

	}


	OnP2LoginStatusChange.ExecuteIfBound(this);
}

void UP2Login::OnLocalRequestClientP2Key(UP2Request* Response)
{
	check(Response != nullptr);
	P2_LOG_I(TEXT("login services: local request was successful: %i"), (Response->ResponseCode == EP2SResponseCode::Success) ? "true" : "false");
	OnP2LoginStatusChange.ExecuteIfBound(this);
}


bool UP2Login::IsWaitingForUserToAuth() const
{
	return AuthPoolingInfo.TimerHandle.IsValid();
}

void UP2Login::StopPoolingForAuthDevice()
{
	if (AuthPoolingInfo.TimerHandle.IsValid())
	{
		P2Subsystem->GetGameInstance()->GetTimerManager().ClearTimer(AuthPoolingInfo.TimerHandle);
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
	if (ClientDeviceObj != nullptr && ClientDeviceObj->ResponseCode == EP2SResponseCode::Success)
	{
		StopPoolingForAuthDevice();
		AuthPoolingInfo.AuthFlow = ClientDeviceObj->AuthFlowData;
		AuthPoolingInfo.StartTime = FDateTime::UtcNow().ToUnixTimestampDecimal();
		
		FTimerDelegate pollDelegate;
		pollDelegate.BindUObject(this, &UP2Login::OnPollForDeviceNew);
		P2Subsystem->GetGameInstance()->GetTimerManager().SetTimer(AuthPoolingInfo.TimerHandle, pollDelegate, AuthPoolingInfo.AuthFlow.interval,true, AuthPoolingInfo.AuthFlow.interval);
		

		P2_LOG_I(TEXT("Starting up login request was accepted. Starting next steps."));
		P2_LOG_V(TEXT("AuthData: {%s}"),*P2::JsonToString(AuthPoolingInfo.AuthFlow.ToJson()));
	}
	else
	{
		P2_LOG_W(TEXT("Starting up login request was denied. Dropping next steps.[%s]"), (ClientDeviceObj ? *StaticEnum<EP2SResponseCode>()->GetNameStringByValue((int64)ClientDeviceObj->ResponseCode)  :TEXT("")));
	}

	OnP2LoginStatusChange.ExecuteIfBound(this);
}

void UP2Login::OnPollForDeviceNew()
{
	if (ClientDeviceObj == nullptr) { return; }

	FP2SchemaAuthFlow& AuthFlow = ClientDeviceObj->AuthFlowData;

	//AuthFlow.

}