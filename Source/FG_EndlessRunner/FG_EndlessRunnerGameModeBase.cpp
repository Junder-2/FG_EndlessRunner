// Copyright Epic Games, Inc. All Rights Reserved.


#include "FG_EndlessRunnerGameModeBase.h"

#include "RunnerCharacter.h"
#include "Kismet/GameplayStatics.h"


AFG_EndlessRunnerGameModeBase::AFG_EndlessRunnerGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/RunnerCharacter/Blueprints/BP_RunnerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PrimaryActorTick.bCanEverTick = true;
}

void AFG_EndlessRunnerGameModeBase::BeginPlay()
{
	Super::BeginPlay();	

	RunnerCharacter = Cast<ARunnerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	CurrentGameSpeed = 1.f;
}

void AFG_EndlessRunnerGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//if(IsPaused()) return;

	CurrentGameSpeed += DeltaSeconds*AddedSpeedPerSecond;

	if(RunnerCharacter != nullptr)
	{
		RunnerCharacter->SetMoveSpeedFromMultiplier(CurrentGameSpeed);
	}
}

ARunnerCharacter* AFG_EndlessRunnerGameModeBase::GetRunnerCharacter() const
{
	return RunnerCharacter;
}

float AFG_EndlessRunnerGameModeBase::GetLanePos(int Lane)
{
	return FMath::Lerp(-TotalWidth*.5f, TotalWidth*.5f, (float)Lane/(NumOfLanes-1));
}
