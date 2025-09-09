/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License


/// ---------------------------------------
/// This file contains the V1 schemas found from the Player2 API.
/// Each schema contains ParseJson & ToJson, for convenient serialization.
/// ---------------------------------------
#pragma once



#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "P2Schemas.generated.h"





USTRUCT(BlueprintType, Category = "SimplePlayer2")
struct PLAYER2_API FP2SchemaHealth
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString client_version;

	 bool ParseJson(TSharedPtr<FJsonObject> JsonObject);
	 TSharedPtr<FJsonObject> ToJson() const;
};

/// Used to authenticate web API access.
USTRUCT(BlueprintType, Category = "SimplePlayer2")
struct PLAYER2_API FP2SchemaClientP2Key
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString p2Key;

	bool ParseJson(TSharedPtr<FJsonObject> JsonObject);
	TSharedPtr<FJsonObject> ToJson() const;
};










USTRUCT(BlueprintType,Category = "SimplePlayer2")
struct PLAYER2_API FP2SchemaAuthFlow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString deviceCode;
	UPROPERTY(BlueprintReadOnly) FString userCode;
	UPROPERTY(BlueprintReadOnly) FString verificationUri;
	UPROPERTY(BlueprintReadOnly) FString verificationUriComplete;
	UPROPERTY(BlueprintReadOnly) int32 expiresIn;
	UPROPERTY(BlueprintReadOnly) int32 interval;

	bool ParseJson(TSharedPtr<FJsonObject> JsonObject);
	TSharedPtr<FJsonObject> ToJson() const;
};