/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "Rest/P2RestBase.h"
#include "Core/P2Schemas.h"
#include "P2RestSystem.generated.h"

UCLASS(Category = "Player2", BlueprintType)
class PLAYER2_API UP2RequestSystem : public UP2Request
{
	GENERATED_BODY()

public:
	void OnRequestComplete(EP2SResponseCode Code, const FString& Content) override;

	
	UPROPERTY(BlueprintReadOnly) FP2SchemaHealth Health;

};




UCLASS()
class PLAYER2_API UP2RestSystem : public UP2RestBase
{
	GENERATED_BODY()

public:

	
	UFUNCTION(BlueprintCallable) UP2RequestSystem* RequestSystemHealth();

	virtual FName GetEndPointType() const override;

};
