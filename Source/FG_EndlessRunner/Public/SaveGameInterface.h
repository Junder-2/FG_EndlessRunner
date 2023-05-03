#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveGameInterface.generated.h"

class URunnerSaveGame;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveGameInterface : public UInterface
{
	GENERATED_BODY()
};

class FG_ENDLESSRUNNER_API ISaveGameInterface
{
	GENERATED_BODY()

protected:
	virtual void SaveNewHighScore(int NewHighScore);
	virtual int LoadHighScore();
	
	TObjectPtr<URunnerSaveGame> SaveGame;
};