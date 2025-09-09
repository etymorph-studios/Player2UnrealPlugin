/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "Core/P2GenericInfo.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HttpModule.h" // Required for FHttpModule
#include "Interfaces/IHttpRequest.h" // Required for HTTP requests
#include "Interfaces/IHttpResponse.h" // Required for IHttpResponse
#include "P2RestBase.generated.h"

class UP2Subsystem;

DECLARE_DELEGATE_OneParam(FOnP2RequestCompleteInternal, class UP2Request*);



namespace P2
{
	EP2SResponseCode ConvertResponseCodeToEnum(int32 responseCode);


	/// Will pick the best URL based on the context of the input
	FString GetPlayer2BaseURL(UObject* Context);

	/// retrieve the base URL for the locally running machine.
	FString GetPlayer2BaseURLLocal(UObject* Context);

	/// retrieve the base URL for cloud version of player2.
	FString GetPlayer2BaseURLCloud(UObject* Context);

	/// Used in web request to give credit to our game.
	inline const FString HeaderPlayer2GameKey = TEXT("player2-game-key");

	FString GetPlayer2GameKey(UObject* Context);

}




UCLASS(Category = "Player2", BlueprintType)
class PLAYER2_API UP2Request : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly) EP2RequestMethod CallMethod { EP2RequestMethod::Get };

	/// The genre of the API.
	UPROPERTY(BlueprintReadOnly) FString EndpointType;

	/// The endpoint type could have multiple methods
	UPROPERTY(BlueprintReadOnly) FString EndpointMethodName;

	/// The endpoint type could have multiple methods
	UPROPERTY(BlueprintReadOnly) EP2SResponseCode ResponseCode { EP2SResponseCode::Undefined };



	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnP2RequestComplete, UP2Request*, ThisResponse);
	/// Assign to this after the proxy returns this object. Will be able to react once the message gets returned.
	UPROPERTY(BlueprintAssignable) FOnP2RequestComplete OnP2RequestComplete;

	/// UP2RestBase will bind to this.
	FOnP2RequestCompleteInternal OnP2RequestCompleteInternal;






	/// Hooking point to handle request after receiving the response.
	virtual void OnRequestComplete(EP2SResponseCode Code, const FString& Content);




	UFUNCTION(BlueprintPure) bool IsCurrentlyRequesting() const;


	/// Proxies must call this prior to starting this classes request.
	void InsertHttpPointer(TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& RequestPointer);
	

	/// Can begin request once the components are initiated.
	///@return True if the request was successfully started.
	bool BeginRequest();



	/// Cleans up any resources used with the rest request.
	virtual void Cleanup();
	virtual void BeginDestroy() override;

private:
	void OnRequestCompleteInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void RequestCompleted(EP2SResponseCode Response);

	/// Flag represents if this object is currently out for a HTTP request.
	bool RequestingFlag{ false };

	/// UE implementation of web requests.
	FHttpRequestPtr HttpRequestPointer;

};









/// This class represents a proxy for creating certain request calls to Player2.
/// Each request call will return a UP2Request class. The user can bind to events and check for request success from this object.
UCLASS(Category = "Player2")
class PLAYER2_API UP2RestBase : public UObject
{
	GENERATED_BODY()


public:
	/// Defines what this request proxy represents.
	UFUNCTION(BlueprintCallable) virtual FName GetEndPointType() const;

	/// Checks if there is at least one instance of the request being called. 
	UFUNCTION(BlueprintPure) bool IsCurrentlyMakingRequestFor(const FString& EndpointMethodName, EP2RequestMethod MethodType) const;

	/// Hooking point to implement business logic after receiving responses from requests.
	virtual void OnRequestCompleted(UP2Request* Request);

	
	/// Called internally within our subsystem
	template<class T>
	static T* CreateRest(UP2Subsystem* Subsystem)
	{
		check(Subsystem != nullptr);
		UP2RestBase* Rest = NewObject<T>();
		Rest->P2Subsystem = Subsystem;
		return Cast<T>(Rest);
	}


private:
	void OnRequestCompletedInternal(UP2Request* Request);


protected:

	void AddRequestToList(UP2Request* Request);
	void RemoveRequestFromList(UP2Request* Request);

	/// Proxies are coupled with the subsystem. Used mostly for broadcasting of events.
	UPROPERTY(Transient) UP2Subsystem* P2Subsystem { nullptr };

	/// Each proxy can contain multiple requests.
	UPROPERTY() TArray<TObjectPtr<UP2Request>> CurrentRequests;

};