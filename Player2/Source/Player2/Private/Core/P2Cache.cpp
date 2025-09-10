/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Core/P2Cache.h"
#include "Kismet/GameplayStatics.h"


namespace P2
{
	inline const FString CacheFileName = TEXT("_P2Cache_");
}

void UP2Cache::InitSaveFile()
{
	UP2Cache* LoadedSaveGame = Cast<UP2Cache>(UGameplayStatics::LoadGameFromSlot(P2::CacheFileName, 0));
	if (LoadedSaveGame == nullptr)
	{
		LoadedSaveGame = Cast<UP2Cache>(UGameplayStatics::CreateSaveGameObject(UP2Cache::StaticClass()));
		check(LoadedSaveGame != nullptr);
		if (LoadedSaveGame != nullptr)
		{
			UGameplayStatics::SaveGameToSlot(LoadedSaveGame,P2::CacheFileName,0);
		}
	}
}

FString UP2Cache::GetBearerToken()
{
	UP2Cache* LoadedSaveGame = Cast<UP2Cache>(UGameplayStatics::LoadGameFromSlot(P2::CacheFileName, 0));
	if (LoadedSaveGame != nullptr)
	{
		return LoadedSaveGame->BearerToken;
	}
	return "";
}

void UP2Cache::SetBearerToken(const FString NewToken)
{
	UP2Cache* LoadedSaveGame = Cast<UP2Cache>(UGameplayStatics::LoadGameFromSlot(P2::CacheFileName, 0));
	if (LoadedSaveGame == nullptr)
	{
		InitSaveFile();
		LoadedSaveGame = Cast<UP2Cache>(UGameplayStatics::LoadGameFromSlot(P2::CacheFileName, 0));
	}
	check(LoadedSaveGame != nullptr);

	if (LoadedSaveGame != nullptr)
	{
		LoadedSaveGame->BearerToken = NewToken;
		UGameplayStatics::SaveGameToSlot(LoadedSaveGame, P2::CacheFileName, 0);
	}

}