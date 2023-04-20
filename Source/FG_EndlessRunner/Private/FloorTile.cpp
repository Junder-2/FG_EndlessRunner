// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorTile.h"

#include "LevelManager.h"
#include "StaticObstacle.h"
#include "Components/BoxComponent.h"

// Sets default values
AFloorTile::AFloorTile()
{
	FloorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FloorCollision"));
	FloorCollision->InitBoxExtent(FVector(1000.f, 500.f, 1.f));
	FloorCollision->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	RootComponent = FloorCollision;
 	
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFloorTile::BeginPlay()
{
	Super::BeginPlay();
	
	LevelManager = ALevelManager::GetLevelManager(GetWorld());

	GenerateObstacles();
}

void AFloorTile::GenerateObstacles()
{
	const int NumOfLanes = LevelManager->NumOfLanes;
	const float ObstacleDifficulty = LevelManager->ObstacleDifficulty;
	
	GeneratedObstacles.Init(EObstacleType::None, ObstacleRowAmount*NumOfLanes);

	const int SpawnAmount = FMath::RandRange(ObstacleDifficulty-2, ObstacleDifficulty+2);

	for (int i = 0; i < SpawnAmount; i++)
	{
		int X = FMath::RandRange(0, NumOfLanes-1);
		int Y = FMath::RandRange(0, ObstacleRowAmount-1);
		const EObstacleType ObstacleType = static_cast<EObstacleType>(FMath::RandRange(1, static_cast<int>(EObstacleType::Num)-1));

		int Failed = 0;

		while (Failed < 10)
		{
			Failed++;

			if(CanSpawnObstacle(X, Y, ObstacleType))
			{
				SpawnObstacle(X, (float)Y/(ObstacleRowAmount-1), ObstacleType);
				GeneratedObstacles[X + Y*NumOfLanes] = ObstacleType;
				break;
			}
			
			X = FMath::RandRange(0, NumOfLanes-1);
			Y = FMath::RandRange(0, ObstacleRowAmount-1);
		}
	}
}

bool AFloorTile::CanSpawnObstacle(const int X, const int Y, const EObstacleType ObstacleType)
{
	const int NumOfLanes = LevelManager->NumOfLanes;
	
	if(GeneratedObstacles[X + Y*NumOfLanes] != EObstacleType::None) return false;

	if(ObstacleType == EObstacleType::Tall)
	{
		int OpenSpot;
		const int AlreadyInRow = CheckObstacleLane(Y, OpenSpot);
		if(AlreadyInRow >= NumOfLanes-1) return false;
		if(AlreadyInRow == NumOfLanes-2)
		{
			if(CheckObstacleExists(OpenSpot, Y-1) != EObstacleType::None) return false;
			if(CheckObstacleExists(OpenSpot, Y+1) != EObstacleType::None) return false;
		}

		if(CheckObstacleExists(X, Y-1) == EObstacleType::None)
		{
			if(CheckObstacleExists(X, Y-2) != EObstacleType::None) return false;
		}
		else if(CheckObstacleExists(X, Y+1) == EObstacleType::None)
		{
			if(CheckObstacleExists(X, Y+2) != EObstacleType::None) return false;
		}
	}
	else if(ObstacleType == EObstacleType::Short)
	{
		if(CheckObstacleExists(X, Y-1) != EObstacleType::None) return false;
		if(CheckObstacleExists(X, Y-2) != EObstacleType::None) return false;
		if(CheckObstacleExists(X, Y+1) != EObstacleType::None) return false;
		if(CheckObstacleExists(X, Y+2) != EObstacleType::None) return false;

		if(CheckObstacleExists(X+1, Y+1) != EObstacleType::None) return false;
		if(CheckObstacleExists(X-1, Y-1) != EObstacleType::None) return false;
		if(CheckObstacleExists(X+1, Y-1) != EObstacleType::None) return false;
		if(CheckObstacleExists(X-1, Y+1) != EObstacleType::None) return false;
	}

	return true;
}

EObstacleType AFloorTile::CheckObstacleExists(const int X, const int Y)
{
	const int NumOfLanes = LevelManager->NumOfLanes;
	
	if(X < 0 || X > NumOfLanes-1) return EObstacleType::Invalid;
	if(Y < 0 || Y > ObstacleRowAmount-1) return EObstacleType::Invalid;
	
	return GeneratedObstacles[X + Y*NumOfLanes];
}

int AFloorTile::CheckObstacleLane(const int Y, int& OpenSpot)
{
	const int NumOfLanes = LevelManager->NumOfLanes;

	int NumberOfBlocking = 0;

	for (int X = 0; X < NumOfLanes; X++)
	{
		if(CheckObstacleExists(X, Y) != EObstacleType::None) NumberOfBlocking++;
		else OpenSpot = X;
	}

	return NumberOfBlocking;
}

int AFloorTile::GetObstacleLaneOpening(const int Y)
{
	const int NumOfLanes = LevelManager->NumOfLanes;

	int NumberOfBlocking = 0;

	for (int X = 0; X < NumOfLanes; X++)
	{
		if(CheckObstacleExists(X, Y) != EObstacleType::None) NumberOfBlocking++;
	}

	return NumberOfBlocking;
}

bool AFloorTile::CheckValidObstaclePlacement(const int X, const int Y, const EObstacleType ObstacleType)
{
	const int NumOfLanes = LevelManager->NumOfLanes;
	
	if(X < 0 || X > NumOfLanes-1) return true;
	if(Y < 0 || Y > ObstacleRowAmount-1) return true;

	switch (GeneratedObstacles[X + Y*NumOfLanes])
	{
		case EObstacleType::None: return true;
		case EObstacleType::Short: return ObstacleType != EObstacleType::Short;
		case EObstacleType::Tall: return false;
	}
	
	return false;
}

void AFloorTile::SpawnObstacle(const int Lane, const float NormalizedRow, const EObstacleType ObstacleType)
{
	AStaticObstacle* NewObstacle;

	switch (ObstacleType)
	{
		case EObstacleType::Short: NewObstacle = ShortObstacle.GetDefaultObject(); break;
		case EObstacleType::Tall: NewObstacle = TallObstacle.GetDefaultObject(); break;
		default: return;
	}
	
	FVector Position;
	float StartOffset = GetExtent()/ObstacleRowAmount;
	Position.X = FMath::Lerp(GetTileStart().X+StartOffset, GetTileEnd().X, NormalizedRow);
	Position.Y = LevelManager->GetLanePos(Lane);
	Position.Z = NewObstacle->GetUpwardsOffset().Z;
	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = Owner;

	AStaticObstacle* SpawnedObstacle = GetWorld()->SpawnActor<AStaticObstacle>(NewObstacle->GetClass(), Position, FRotator::ZeroRotator, SpawnInfo);
	SpawnedObstacle->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

void AFloorTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	AddActorWorldOffset(GetActorForwardVector()*(-LevelManager->GetMoveSpeed()*DeltaTime));
}

float AFloorTile::GetExtent() const
{
	return FloorCollision->GetScaledBoxExtent().X;
}

FVector AFloorTile::GetTileStart() const
{
	return GetActorLocation() - GetExtent()*FVector::ForwardVector;
}

FVector AFloorTile::GetTileEnd() const
{
	return GetActorLocation() + GetExtent()*FVector::ForwardVector;
}

FVector AFloorTile::GetPositionFromStart(const FVector Start) const
{
	return Start + GetExtent()*FVector::ForwardVector;
}
