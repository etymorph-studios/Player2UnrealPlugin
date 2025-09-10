/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Delegates/Delegate.h"
#include "Core/P2Schemas.h"
#include "P2Subsystem.generated.h"

class UP2RestBase;
class UP2Request;
class UP2RequestSystem;
class UP2RequestClientP2Key;
class UP2Login;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnP2SystemChange);


/// This subsystem handles the overhead of starting up player2 services.
/// Also, this class acts as the hub for both events and data to be stored with respect to player2 functionality.
UCLASS(Category = "Player2", Config = Game, defaultconfig, meta = (DisplayName = "Player2 Subsystem"))
class PLAYER2_API UP2Subsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/// Bind to be notify when the state of player2 health changes
	UPROPERTY(BlueprintAssignable) FOnP2SystemChange OnSystemHealthChange;
	void UpdateRequestSystem(UP2RequestSystem* LastRequest);

	/// Bind to be notify when the client key has been either authenticated or unauthenticated.
	/// Listen to UP2Login events for more fine grain status updates. 
	UPROPERTY(BlueprintAssignable) FOnP2SystemChange OnClientLoginAuthenticationUpdate;


	/// @return true if player2 is alive and healthy for requests.
	UFUNCTION(BlueprintPure) bool IsPlayer2Healthy() const;

	UFUNCTION(BlueprintPure) UP2Login* GetP2Login() const;


	/// @return true if the request was sent out.
	UFUNCTION(BlueprintCallable) bool ForceCheckSystemHealth();

private:
	UFUNCTION() void InitDelayNext();

	UFUNCTION() void OnSystemHealthHeartbeat();
	UFUNCTION() void OnRequestHealthHeartbeatComplete(UP2Request* Response);
	FTimerHandle HandleForSystemHeartbeat;

	
	UFUNCTION() void OnLoginStatusChanged(UP2Login* Login, bool IsAuthenticated);


	/// List of REST proxies.
	UPROPERTY() TArray<TObjectPtr<UP2RestBase>> RestEndpoints;
	
	/// Contains the last interaction with the system health API.
	UPROPERTY(Transient) UP2RequestSystem* LastRequestSystem { nullptr };

	
	UPROPERTY(Transient) UP2Login* P2Login { nullptr };




public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	template<class T>
	inline T* GetRestEndpointRaw() const
	{
		for (TObjectPtr<UP2RestBase> Obj : RestEndpoints)
		{
			if (T* Target = Cast<T>(Obj); Target != nullptr)
			{
				return Target;
			}
		}
		return nullptr;
	}
};