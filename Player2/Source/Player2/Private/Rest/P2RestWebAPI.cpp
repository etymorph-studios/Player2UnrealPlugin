/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Rest/P2RestWebAPI.h"
#include "Core/P2Logger.h"

namespace {
	FString CreateSerializeGameClientId(const FString& clientId)
	{
		return FString::Printf(TEXT("{\"client_id\":\"%s\"}"), *clientId);
	}
}


void UP2RequestClientP2Key::OnRequestComplete(EP2SResponseCode Code, const FString& Content)
{
	P2Key = FP2SchemaClientP2Key();
	if (Code == EP2SResponseCode::Success)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			P2Key.ParseJson(JsonObject);
			P2_LOG_V(TEXT("P2Key is %s"), *P2Key.p2Key);
		}
		else
		{
			P2_LOG_W(TEXT("InValid Json Object for ClientP2Key"));
		}
	}
}

void UP2RequestLoginDevice::OnRequestComplete(EP2SResponseCode Code, const FString& Content)
{
	AuthFlowData = FP2SchemaAuthFlow();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		AuthFlowData.ParseJson(JsonObject);
	}
	else
	{
		P2_LOG_W(TEXT("InValid Json Object for FP2SchemaAuthFlow"));
	}
}



FName UP2RestWeb::GetEndPointType() const
{
	return TEXT("web");
}


UP2RequestClientP2Key* UP2RestWeb::RequestLocalClientP2Key()
{
	UP2RequestClientP2Key* Request = NewObject<UP2RequestClientP2Key>();
	Request->CallMethod = EP2RequestMethod::Get;
	Request->EndpointType = GetEndPointType().ToString();
	Request->EndpointMethodName = FString::Printf(TEXT("v1/login/web/%s"),*P2::GetPlayer2GameKey(this));
	Request->LocalMachineCallFlag = true;

	FHttpModule& HttpMod = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> NewRequest = HttpMod.CreateRequest();
	NewRequest->SetURL(P2::GetPlayer2BaseURLLocal(this) + TEXT("/") + Request->EndpointMethodName);
	NewRequest->SetVerb(TEXT("POST"));
	NewRequest->SetHeader(TEXT("accept"), TEXT("application/json"));
	NewRequest->SetHeader(P2::HeaderPlayer2GameKey, P2::GetPlayer2GameKey(this));
	NewRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));

	Request->InsertHttpPointer(NewRequest);


	if (bool RequestStarted = Request->BeginRequest(); RequestStarted)
	{
		AddRequestToList(Request);
		PLAYER2_LOG(Log, TEXT("RequestLocalClientP2Key Message requesting to %s"), *NewRequest->GetURL());
	}
	else
	{
		PLAYER2_LOG(Warning, TEXT("RequestLocalClientP2Key failed to send request over to the server."));
		OnRequestCompleted(Request);
	}

	return Request;
}

UP2RequestClientP2Key* UP2RestWeb::RequestCloudClientP2Key()
{
	UP2RequestClientP2Key* Request = NewObject<UP2RequestClientP2Key>();
	Request->CallMethod = EP2RequestMethod::Get;
	Request->EndpointType = GetEndPointType().ToString();
	Request->EndpointMethodName = FString::Printf(TEXT("v1/login/web/%s"), *P2::GetPlayer2GameKey(this));
	Request->LocalMachineCallFlag = false;

	FHttpModule& HttpMod = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> NewRequest = HttpMod.CreateRequest();
	NewRequest->SetURL(P2::GetPlayer2BaseURLCloud((UObject*)P2Subsystem) + TEXT("/") + Request->EndpointMethodName);
	NewRequest->SetVerb(TEXT("POST"));
	NewRequest->SetHeader(TEXT("accept"), TEXT("application/json"));
	NewRequest->SetHeader(P2::HeaderPlayer2GameKey, P2::GetPlayer2GameKey(this));
	NewRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));

	Request->InsertHttpPointer(NewRequest);


	if (bool RequestStarted = Request->BeginRequest(); RequestStarted)
	{
		AddRequestToList(Request);
		PLAYER2_LOG(Log, TEXT("RequestCloudClientP2Key Message requesting to %s"), *NewRequest->GetURL());
	}
	else
	{
		PLAYER2_LOG(Warning, TEXT("RequestCloudClientP2Key failed to send request over to the server."));
		OnRequestCompleted(Request);
	}

	return Request;
}


UP2RequestLoginDevice* UP2RestWeb::RequestLoginDevice()
{
	UP2RequestLoginDevice* Request = NewObject<UP2RequestLoginDevice>();
	Request->CallMethod = EP2RequestMethod::Get;
	Request->EndpointType = GetEndPointType().ToString();
	Request->EndpointMethodName = TEXT("v1/login/device/new");

	FHttpModule& HttpMod = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> NewRequest = HttpMod.CreateRequest();
	NewRequest->SetURL(P2::GetPlayer2BaseURLCloud((UObject*)P2Subsystem) + TEXT("/") + Request->EndpointMethodName);
	NewRequest->SetVerb(TEXT("POST"));
	NewRequest->SetHeader(TEXT("accept"), TEXT("application/json"));
	NewRequest->SetHeader(P2::HeaderPlayer2GameKey, P2::GetPlayer2GameKey(this));
	NewRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));
	
	NewRequest->SetContentAsString(::CreateSerializeGameClientId(P2::GetPlayer2GameKey(this)));


	Request->InsertHttpPointer(NewRequest);


	if (bool RequestStarted = Request->BeginRequest(); RequestStarted)
	{
		AddRequestToList(Request);
		PLAYER2_LOG(Log, TEXT("RequestLoginDevice Message requesting to %s"), *NewRequest->GetURL());
	}
	else
	{
		PLAYER2_LOG(Warning, TEXT("RequestLoginDevice failed to send request over to the server."));
		OnRequestCompleted(Request);
	}

	return Request;

}