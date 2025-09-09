/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "P2DeveloperSettings.generated.h"



/// Define global variables used within the Player2 Subsystem. Variables can be modified within the editor settings.
UCLASS(Category = "Player2", Config = Game, defaultconfig, meta = (DisplayName = "Player2 Settings"))
class PLAYER2_API UP2DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/// Unique Key (typically the game name) used to reference the application when calling the Player2 API.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, AdvancedDisplay) FString HeaderPlayer2GameKey;

	/// The listening address used to catch Player2 Authorization Code Flow hook call
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, AdvancedDisplay) FString HookAuthorizationURL;
};