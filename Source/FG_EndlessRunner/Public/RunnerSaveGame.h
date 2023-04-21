#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RunnerSaveGame.generated.h"

UCLASS()
class FG_ENDLESSRUNNER_API URunnerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int HighScore;
};
