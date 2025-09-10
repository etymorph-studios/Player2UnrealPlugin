/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Rest/P2RestSystem.h"
#include "Core/P2Logger.h"
#include "Core/P2Subsystem.h"
#include "Rest/P2Login.h"

void UP2RequestSystem::OnRequestComplete(EP2SResponseCode Code, const FString& Content)
{
	Health = FP2SchemaHealth();
	if (Code == EP2SResponseCode::Success)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			if (!Health.ParseJson(JsonObject))
			{
				Health.client_version = TEXT("0.0");
			}
		}
		else
		{
			P2_LOG_W(TEXT("InValid Json Object for system health"));
		}
	}
	else
	{
			P2_LOG_W(TEXT("System health check failed[ %s ]"), *StaticEnum<EP2SResponseCode>()->GetNameStringByValue((int64)Code));
	}
}









UP2RequestSystem* UP2RestSystem::RequestSystemHealth()
{
	UP2RequestSystem* Request = NewObject<UP2RequestSystem>();
	Request->CallMethod = EP2RequestMethod::Get;
	Request->EndpointType = GetEndPointType().ToString();
	Request->EndpointMethodName = TEXT("v1/health");


	FHttpModule& HttpMod = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> NewRequest = HttpMod.CreateRequest();
	NewRequest->SetURL(P2::GetPlayer2BaseURL(P2Subsystem) + TEXT("/") + Request->EndpointMethodName);
	NewRequest->SetVerb(TEXT("GET"));
	NewRequest->SetHeader(TEXT("accept"), TEXT("application/json; charset=utf-8"));
	NewRequest->SetHeader(P2::HeaderPlayer2GameKey, P2::GetPlayer2GameKey(P2Subsystem));
	//NewRequest->SetHeader(TEXT("p2Key"), P2Subsystem->GetP2Login()->GetClientP2Key());

	switch(P2Subsystem->GetP2Login()->GetPlayer2AuthenticationMode())
	{
		case EP2AuthentMode::Local: break;
		case EP2AuthentMode::Bearer: 
			P2_LOG_V(TEXT("Adding bearer key to system health call"));
			//NewRequest->SetContentAsString(FString::Printf(TEXT("{\"bearer\":\"%s\"}"), *P2Subsystem->GetP2Login()->GetClientP2Key()));

			//NewRequest->SetHeader(TEXT("bearer"), P2Subsystem->GetP2Login()->GetClientP2Key());
			NewRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"),*P2Subsystem->GetP2Login()->GetClientP2Key()));
			break;
		case EP2AuthentMode::ApiToken: break;
	}

	Request->InsertHttpPointer(NewRequest);
	

	if (bool RequestStarted = Request->BeginRequest(); RequestStarted)
	{
		AddRequestToList(Request);
		PLAYER2_LOG(Log, TEXT("RequestSystemHealth Message requesting."));
	}
	else
	{
		PLAYER2_LOG(Warning, TEXT("RequestSystemHealth failed to send request over to the server."));
		OnRequestCompleted(Request);
	}

	return Request;
}


FName UP2RestSystem::GetEndPointType() const
{
	return TEXT("system");
}
