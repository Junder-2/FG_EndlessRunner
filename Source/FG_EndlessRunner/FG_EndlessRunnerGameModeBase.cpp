// Copyright Epic Games, Inc. All Rights Reserved.


#include "FG_EndlessRunnerGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AFG_EndlessRunnerGameModeBase::AFG_EndlessRunnerGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/RunnerCharacter/Blueprints/BP_RunnerCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

