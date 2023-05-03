#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EndlessRunnerGameInstance.generated.h"

UCLASS()
class FG_ENDLESSRUNNER_API UEndlessRunnerGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEndlessRunnerGameInstance();

	UPROPERTY(BlueprintReadWrite)
	bool bTwoPlayerGame;	
};