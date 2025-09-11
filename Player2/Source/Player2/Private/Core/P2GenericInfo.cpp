/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Core/P2GenericInfo.h"
#include "Serialization/JsonSerializer.h"


namespace P2
{
	constexpr int32 PluginMajor = 1;
	constexpr int32 PluginMinor = 1;
	constexpr int32 PluginPatch = 1;

	void PluginVersion(int32& Major, int32& Minor, int32& Patch)
	{
		Major = PluginMajor;
		Minor = PluginMinor;
		Patch = PluginPatch;
	}
	FString PluginVersion()
	{
		return FString::Printf(TEXT("%i.%i.%i"), PluginMajor, PluginMinor, PluginPatch);
	}


	FString JsonToString(const TSharedPtr<FJsonObject>& JsonObj)
	{
		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
		return OutputString;
	}

}