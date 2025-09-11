/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Player2BPLibrary.generated.h"


/// Public facing API for game development
/// Acts as a bridge for complex subsystem calls.
UCLASS(Category = "Player2")
class PLAYER2_API UPlayer2BPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, Category = "Player2|Library") static FString GetPlayer2PluginVersion();

	/// Checks if there is an active connection with the player2 client.
	UFUNCTION(BlueprintPure, Category = "Player2|Library", meta = (WorldContext = "WorldContextObject"))
	static bool IsPlayer2Healthy(UObject* WorldContextObject);
};
