// Copyright Epic Games, Inc. All Rights Reserved.


#include "FG_EndlessRunnerGameModeBase.h"

AFG_EndlessRunnerGameModeBase::AFG_EndlessRunnerGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/RunnerCharacter/Blueprints/BP_RunnerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
