// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManager.h"

#include "FloorTile.h"
#include "RunnerCharacter.h"
#include "StaticObstacle.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALevelManager::ALevelManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelManager::BeginPlay()
{
	Super::BeginPlay();	

	RunnerCharacter = Cast<ARunnerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	CurrentGameSpeed = 1.f;

	for (int i = 0; i < 10; i++)
	{
		SpawnRandomFloorTile();
	}
}

void ALevelManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//if(IsPaused()) return;

	CurrentGameSpeed += DeltaSeconds*AddedSpeedPerSecond;

	if(RunnerCharacter != nullptr)
	{
		RunnerCharacter->SetMoveSpeed(GetMoveSpeed());
	}

	if(!CurrentFloorTiles.IsEmpty())
	{
		if(CurrentFloorTiles.First()->GetActorLocation().X < -5000.f)
		{
			TArray<AActor*> temp;
			CurrentFloorTiles.First()->GetAttachedActors(temp);
			
			for (AActor* CActor : temp)
			{
				CActor->Destroy();
			}
			CurrentFloorTiles.First()->Destroy();
			
			CurrentFloorTiles.PopFirst();
			SpawnRandomFloorTile();
		}
	}
}

ARunnerCharacter* ALevelManager::GetRunnerCharacter() const
{
	return RunnerCharacter;
}

float ALevelManager::GetRandomLanePos() const
{
	return GetLanePos(FMath::RandRange(0, NumOfLanes-1));
}

float ALevelManager::GetLanePos(int Lane) const
{
	return FMath::Lerp(-TotalWidth*.5f, TotalWidth*.5f, (float)Lane/(NumOfLanes-1));
}

int ALevelManager::GetLane(FVector Location) const
{
	return UKismetMathLibrary::NormalizeToRange(Location.Y, -TotalWidth*.5f, TotalWidth*.5f) * (NumOfLanes-1);
}

float ALevelManager::GetMoveSpeed() const
{
	return BaseMoveSpeed * CurrentGameSpeed;
}

ALevelManager* ALevelManager::GetLevelManager(const UObject* WorldContextObject)
{
	return Cast<ALevelManager>(UGameplayStatics::GetActorOfClass(WorldContextObject, StaticClass()));
}

void ALevelManager::SpawnRandomFloorTile()
{
	if(FloorTilesPrefabs.IsEmpty()) return;

	const AFloorTile* NewFloorTile = FloorTilesPrefabs[FMath::RandRange(0, FloorTilesPrefabs.Num()-1)].GetDefaultObject();
	
	FVector Position = FVector(-NewFloorTile->GetExtent(), 0, 0);
	
	if(!CurrentFloorTiles.IsEmpty())
	{
		const AFloorTile* LastFloorTile = CurrentFloorTiles.Last();
		Position = NewFloorTile->GetPositionFromStart(LastFloorTile->GetTileEnd());
	}

	const FActorSpawnParameters SpawnInfo;

	CurrentFloorTiles.PushLast(GetWorld()->SpawnActor<AFloorTile>(NewFloorTile->GetClass(), Position, FRotator::ZeroRotator, SpawnInfo));
}
