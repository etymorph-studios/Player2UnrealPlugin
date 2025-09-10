/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "GameFramework/SaveGame.h"
#include "P2Cache.generated.h"

/// Place to save necessary data for web api calls and other player2 preferences.
/// Class methods are intended to be used as global getters/setters. 
UCLASS(Category = "Player2")
class UP2Cache : public USaveGame
{
	GENERATED_BODY()

	// Used in the initial setup to check if we have a prior valid state.
	UPROPERTY() FString BearerToken;

public:



	static void InitSaveFile();

	static FString GetBearerToken();
	static void SetBearerToken(const FString NewToken);

	//static FString GetBearerToken();
};