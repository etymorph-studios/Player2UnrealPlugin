/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Core/P2Schemas.h"

#include "Serialization/JsonSerializer.h" // Required for FJsonSerializer













bool FP2SchemaHealth::ParseJson(TSharedPtr<FJsonObject> JsonObject)
{
	check(JsonObject.IsValid());
	JsonObject->TryGetStringField(TEXT("client_version"), client_version);
	return !client_version.IsEmpty();
}
TSharedPtr<FJsonObject> FP2SchemaHealth::ToJson() const
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("client_version"), client_version);
	return JsonObject;
}


bool FP2SchemaClientP2Key::ParseJson(TSharedPtr<FJsonObject> JsonObject)
{
	check(JsonObject.IsValid());
	JsonObject->TryGetStringField(TEXT("p2Key"), p2Key);
	return !p2Key.IsEmpty();
}
TSharedPtr<FJsonObject> FP2SchemaClientP2Key::ToJson() const
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("p2Key"), p2Key);
	return JsonObject;
}


bool FP2SchemaAuthFlow::ParseJson(TSharedPtr<FJsonObject> JsonObject)
{
	check(JsonObject.IsValid());
	JsonObject->TryGetStringField(TEXT("deviceCode"), deviceCode);
	JsonObject->TryGetStringField(TEXT("userCode"), userCode);
	JsonObject->TryGetStringField(TEXT("verificationUri"), verificationUri);
	JsonObject->TryGetStringField(TEXT("verificationUriComplete"), verificationUriComplete);
	JsonObject->TryGetNumberField(TEXT("expiresIn"), expiresIn);
	JsonObject->TryGetNumberField(TEXT("interval"), interval);
	return !deviceCode.IsEmpty() && !userCode.IsEmpty();
}
TSharedPtr<FJsonObject> FP2SchemaAuthFlow::ToJson() const
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("deviceCode"), deviceCode);
	JsonObject->SetStringField(TEXT("userCode"), userCode);
	JsonObject->SetStringField(TEXT("verificationUri"), verificationUri);
	JsonObject->SetStringField(TEXT("verificationUriComplete"), verificationUriComplete);
	JsonObject->SetNumberField(TEXT("expiresIn"), expiresIn);
	JsonObject->SetNumberField(TEXT("interval"), interval);
	return JsonObject;
}