#include "SaveGameInterface.h"

#include "RunnerSaveGame.h"
#include "Kismet/GameplayStatics.h"

#define SLOT_NAME "SaveSlot"

int ISaveGameInterface::LoadHighScore()
{
	if(UGameplayStatics::DoesSaveGameExist(SLOT_NAME, 0))
	{
		SaveGame = Cast<URunnerSaveGame>(UGameplayStatics::LoadGameFromSlot(SLOT_NAME, 0));
		return SaveGame->HighScore;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Save"));
		SaveGame = Cast<URunnerSaveGame>(UGameplayStatics::CreateSaveGameObject(URunnerSaveGame::StaticClass()));
		return 0;
	}
}

void ISaveGameInterface::SaveNewHighScore(const int NewHighScore)
{
	SaveGame = Cast<URunnerSaveGame>(UGameplayStatics::CreateSaveGameObject(URunnerSaveGame::StaticClass()));

	SaveGame->HighScore = NewHighScore;
	UGameplayStatics::SaveGameToSlot(SaveGame, SLOT_NAME, 0);
}