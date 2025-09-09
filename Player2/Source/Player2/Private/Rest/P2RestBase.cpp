/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Rest/P2RestBase.h"
#include "Rest/P2Login.h"
#include "Core/P2DeveloperSettings.h"
#include "Core/P2Subsystem.h"
#include "Core/P2Logger.h"

namespace P2
{
	EP2SResponseCode ConvertResponseCodeToEnum(int32 responseCode)
	{

		switch (responseCode)
		{
		case 200: return EP2SResponseCode::Success;
		case 400: return EP2SResponseCode::InvalidArgument;
		case 401: return EP2SResponseCode::NotAuthenticated;
		case 402: return EP2SResponseCode::InsufficientCredits;
		case 404: return EP2SResponseCode::UnknownPage;
		case 409: return EP2SResponseCode::AlreadyStarted;
		case 429: return EP2SResponseCode::ToManyRequests;
		case 500: return EP2SResponseCode::InternalServerError;
		}
		return EP2SResponseCode::Undefined;

	}

	FString GetPlayer2BaseURL(UObject* Context)
	{
		bool LocalHostRunning = false;
		if (UWorld* World = GEngine->GetWorldFromContextObject(Context, EGetWorldErrorMode::ReturnNull); World != nullptr)
		{
			UP2Subsystem* subsystem = World->GetGameInstance()->GetSubsystem<UP2Subsystem>();
			LocalHostRunning = subsystem->GetP2Login()->IsPlayer2RunningLocally();
		}

		if (LocalHostRunning)
		{
			return GetPlayer2BaseURLLocal(Context);
		}
		else
		{
			return GetPlayer2BaseURLCloud(Context);
		}

	}
	FString GetPlayer2BaseURLLocal(UObject* Context)
	{
		FString Player2Url = TEXT("http://localhost:4315");
		return Player2Url;
	}
	FString GetPlayer2BaseURLCloud(UObject* Context)
	{
		FString Player2Url = TEXT("https://api.player2.game");
		return Player2Url;
	}

	FString GetPlayer2GameKey(UObject* Context)
	{
		const UP2DeveloperSettings* DeveloperSettings = GetDefault<UP2DeveloperSettings>();
		check(DeveloperSettings != nullptr);
		return DeveloperSettings->HeaderPlayer2GameKey;
	}
}



void UP2Request::Cleanup()
{
	if (HttpRequestPointer.IsValid())
	{
		HttpRequestPointer->OnProcessRequestComplete().Unbind();
		HttpRequestPointer.Reset();
	}
}

void UP2Request::BeginDestroy()
{
	Cleanup();
	Super::BeginDestroy();
}

void UP2Request::OnRequestCompleteInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	EP2SResponseCode Code = EP2SResponseCode::Undefined;
	FString Content;

	if (Response.IsValid())
	{
		Content = Response.Get()->GetContentAsString();
		Code = P2::ConvertResponseCodeToEnum(Response->GetResponseCode());

		if (Code == EP2SResponseCode::Undefined)
		{
			P2_LOG_W(TEXT("Received an unknown response code (%i) from %s/%s"), Response->GetResponseCode(), *EndpointType, *EndpointMethodName);
		}
	}

	OnRequestComplete(Code, Content);
	RequestCompleted(Code);
}

void UP2Request::OnRequestComplete(EP2SResponseCode Code, const FString& Content)
{
	// Stub method.
}
void UP2Request::RequestCompleted(EP2SResponseCode Response)
{
	if (RequestingFlag)
	{
		RequestingFlag = false;
		ResponseCode = Response;
		OnP2RequestCompleteInternal.ExecuteIfBound(this);
		OnP2RequestComplete.Broadcast(this);
	}
	Cleanup();
}

bool UP2Request::IsCurrentlyRequesting() const
{
	return RequestingFlag;
}

bool UP2Request::BeginRequest()
{
	if (!RequestingFlag && HttpRequestPointer.IsValid())
	{
		RequestingFlag = HttpRequestPointer->ProcessRequest();
		return RequestingFlag;
	}
	return false;
}

void UP2Request::InsertHttpPointer(TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& RequestPointer)
{
	HttpRequestPointer = RequestPointer.ToSharedPtr();

	if (HttpRequestPointer.IsValid())
	{
		HttpRequestPointer->OnProcessRequestComplete().BindUObject(this, &UP2Request::OnRequestCompleteInternal);
	}
}






///---------------------------------
/// UP2RestBase 
///---------------------------------

FName UP2RestBase::GetEndPointType() const
{
	return NAME_None;
}

bool UP2RestBase::IsCurrentlyMakingRequestFor(const FString& EndpointMethodName, EP2RequestMethod MethodType) const
{
	for (UP2Request* Request : CurrentRequests)
	{
		if (Request->EndpointMethodName == EndpointMethodName && Request->CallMethod == MethodType)
		{
			return true;
		}
	}
	return false;
}

void UP2RestBase::OnRequestCompleted(UP2Request* Request)
{
	// Stub method.
}
void UP2RestBase::OnRequestCompletedInternal(UP2Request* Request)
{
	check(Request != nullptr);
	OnRequestCompleted(Request);
	CurrentRequests.RemoveAt(CurrentRequests.Find(Request));
}

void UP2RestBase::AddRequestToList(UP2Request* Request)
{
	check(Request != nullptr);
	CurrentRequests.Add(Request);
	Request->OnP2RequestCompleteInternal.BindUObject(this, &UP2RestBase::OnRequestCompleted);
}

void UP2RestBase::RemoveRequestFromList(UP2Request* Request)
{
	check(Request != nullptr);
	CurrentRequests.RemoveAt(CurrentRequests.Find(Request));
}