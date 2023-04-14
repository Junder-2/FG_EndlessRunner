// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FG_EndlessRunner/FG_EndlessRunnerGameModeBase.h"
#include "GameFramework/Actor.h"
#include "FloorTile.generated.h"

class AStaticObstacle;
class UBoxComponent;
class ALevelManager;

UCLASS()
class FG_ENDLESSRUNNER_API AFloorTile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> FloorCollision;
	
public:	
	// Sets default values for this actor's properties
	AFloorTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TObjectPtr<ALevelManager> LevelManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile Variables")
	TArray<TSubclassOf<AStaticObstacle>> ObstaclePrefabs;

	void SpawnRandomObstacle();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetExtent() const;
	FVector GetTileStart() const;
	FVector GetTileEnd() const;
	FVector GetPositionFromStart(FVector Start) const;

};
