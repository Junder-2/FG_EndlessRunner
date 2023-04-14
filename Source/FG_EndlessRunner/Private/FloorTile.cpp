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

	for (int i = 0; i < 5; i++)
	{
		SpawnRandomObstacle();
	}
}

void AFloorTile::SpawnRandomObstacle()
{
	if(ObstaclePrefabs.IsEmpty()) return;

	const AStaticObstacle* NewObstacle = ObstaclePrefabs[FMath::RandRange(0, ObstaclePrefabs.Num()-1)].GetDefaultObject();

	FVector Position;
	Position.X = FMath::RandRange(GetTileStart().X, GetTileEnd().X);
	Position.Y = LevelManager->GetRandomLanePos();
	Position.Z = NewObstacle->GetUpwardsOffset().Z;
	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = Owner;

	AStaticObstacle* SpawnedObstacle = GetWorld()->SpawnActor<AStaticObstacle>(NewObstacle->GetClass(), Position, FRotator::ZeroRotator, SpawnInfo);
	SpawnedObstacle->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

// Called every frame
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
