// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorTile.generated.h"

class AStaticObstacle;
class UBoxComponent;
class ALevelManager;

UENUM(BlueprintType)
enum class EObstacleType : uint8
{
	None = 0 UMETA(Hidden),
	Short = 1,
	Tall = 2,
	Num = 3 UMETA(Hidden),
	Invalid = 4 UMETA(Hidden),
};

UCLASS()
class FG_ENDLESSRUNNER_API AFloorTile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> FloorCollision;
	
public:	
	AFloorTile();

protected:
	virtual void BeginPlay() override;

	TObjectPtr<ALevelManager> LevelManager;

	UPROPERTY(EditAnywhere, Category="Tile Variables")
	int ObstacleRowAmount = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile Variables")
	TSubclassOf<AStaticObstacle> TallObstacle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile Variables")
	TSubclassOf<AStaticObstacle> ShortObstacle;
	
	void SpawnObstacle(int Lane, float NormalizedRow, EObstacleType ObstacleType);
	void GenerateObstacles();
	EObstacleType CheckObstacleExists(int X, int Y);
	int CheckObstacleLane(int Y, int& OpenSpot);
	int GetObstacleLaneOpening(int Y);

	bool CanSpawnObstacle(int X, int Y, EObstacleType ObstacleType);
	bool CheckValidObstaclePlacement(int X, int Y, EObstacleType ObstacleType);

	TArray<EObstacleType> GeneratedObstacles;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetExtent() const;
	FVector GetTileStart() const;
	FVector GetTileEnd() const;
	FVector GetPositionFromStart(FVector Start) const;
};