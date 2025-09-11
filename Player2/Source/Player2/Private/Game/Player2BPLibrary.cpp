/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Game/Player2BPLibrary.h"
#include "Core/P2Subsystem.h"
#include "Core/P2GenericInfo.h"

UPlayer2BPLibrary::UPlayer2BPLibrary(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer){}

FString UPlayer2BPLibrary::GetPlayer2PluginVersion()
{
	return P2::PluginVersion();
}

bool UPlayer2BPLibrary::IsPlayer2Healthy(UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); World!=nullptr)
	{
		UP2Subsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UP2Subsystem>();
		check(Subsystem != nullptr);
		return Subsystem->IsPlayer2Healthy();
	}

	return false;
}

