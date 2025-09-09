/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#include "Core/P2GenericInfo.h"
#include "Serialization/JsonSerializer.h"


namespace P2
{
	FString JsonToString(const TSharedPtr<FJsonObject>& JsonObj)
	{
		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
		return OutputString;
	}

}