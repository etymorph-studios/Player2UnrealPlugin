/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License


#include "Core/P2Subsystem.h"
#include "Core/P2Logger.h"
#include "Core/P2Cache.h"
#include "Rest/P2RestSystem.h"
#include "Rest/P2RestWebAPI.h"
#include "Rest/P2Login.h"

//"https://api.player2.game"

void UP2Subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	RestEndpoints.Add(UP2RestBase::CreateRest<UP2RestSystem>(this));
	RestEndpoints.Add(UP2RestBase::CreateRest<UP2RestWeb>(this));

	P2Login = NewObject<UP2Login>();

	//P2Login->OnP2LoginStatusChange.BindUObject(this, &UP2Subsystem::OnSystemHealthHeartbeat);

	GetGameInstance()->GetTimerManager().SetTimer(HandleForSystemHeartbeat, this, &UP2Subsystem::OnSystemHealthHeartbeat, 60, true, 2);

	GetGameInstance()->GetTimerManager().SetTimerForNextTick(this, &UP2Subsystem::InitDelayNext);
	P2_LOG_I(TEXT("Player2 Subsystem Initialized."));
}

void UP2Subsystem::InitDelayNext()
{
	UP2Cache::InitSaveFile();

	FOnP2LoginAuthenticated Delegate;
	Delegate.BindUObject(this, &UP2Subsystem::OnLoginStatusChanged);
	P2Login->InitStartup(this, MoveTemp(Delegate));
}

void UP2Subsystem::Deinitialize()
{
	P2_LOG_I(TEXT("Player2 Subsystem Shutting down."));
	Super::Deinitialize();
}


bool UP2Subsystem::IsPlayer2Healthy() const
{
	return LastRequestSystem!=nullptr && !LastRequestSystem->Health.client_version.IsEmpty(); //placeholder
}



bool UP2Subsystem::ForceCheckSystemHealth()
{
	UP2RestSystem* RestSystem = GetRestEndpointRaw<UP2RestSystem>();
	check(RestSystem != nullptr);
	UP2RequestSystem* SystemHealthObj = RestSystem->RequestSystemHealth();
	check(SystemHealthObj != nullptr);
	if (SystemHealthObj != nullptr && SystemHealthObj->IsCurrentlyRequesting())
	{
		SystemHealthObj->OnP2RequestComplete.AddDynamic(this, &UP2Subsystem::OnRequestHealthHeartbeatComplete);
		return true;
	}
	else
	{
		OnRequestHealthHeartbeatComplete(nullptr);
		return false;
	}
}

void UP2Subsystem::OnSystemHealthHeartbeat()
{
	if (!P2Login->GetClientP2Key().IsEmpty())
	{
		ForceCheckSystemHealth();
	}
	else
	{
		P2_LOG_I(TEXT("Player2 can't heartbeat while there is no client P2 Key."));
	}

}
void UP2Subsystem::OnRequestHealthHeartbeatComplete(UP2Request* Response)
{
	UP2RequestSystem* HealthResponse = Cast<UP2RequestSystem>(Response);
	UpdateRequestSystem(HealthResponse);
}

void UP2Subsystem::UpdateRequestSystem(class UP2RequestSystem* LastRequest)
{
	bool LastState = IsPlayer2Healthy();
	LastRequestSystem = LastRequest;
	if (bool NewState = IsPlayer2Healthy(); LastState != NewState)
	{
		OnSystemHealthChange.Broadcast();
	}
	P2_LOG_V( TEXT("Player2 system is %s"), (IsPlayer2Healthy() ? TEXT("healthy") : TEXT("not healthy")) );
}


UP2Login* UP2Subsystem::GetP2Login() const
{
	return P2Login;
}

void UP2Subsystem::OnLoginStatusChanged(UP2Login* Login, bool IsAuthenticated)
{
	check(Login != nullptr);
	if (!IsPlayer2Healthy() && !Login->GetClientP2Key().IsEmpty())
	{
		OnSystemHealthHeartbeat();
	}
	OnClientLoginAuthenticationUpdate.Broadcast();
}




