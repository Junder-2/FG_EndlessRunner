// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Deque.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

class AFloorTile;
class ARunnerCharacter;
UCLASS()
class FG_ENDLESSRUNNER_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelManager();

private:
	TObjectPtr<ARunnerCharacter> RunnerCharacter; 

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Variables")
	int NumOfLanes = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Variables")
	float TotalWidth = 250.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Variables")
	float AddedSpeedPerSecond = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game Variables")
	float BaseMoveSpeed = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Game Variables")
	TArray<TSubclassOf<AFloorTile>> FloorTilesPrefabs;

public:
	UPROPERTY(Transient, BlueprintReadWrite, VisibleAnywhere, Category="Game Properties")
	float CurrentGameSpeed;
	
	ARunnerCharacter* GetRunnerCharacter() const;
	float GetRandomLanePos() const;
	float GetLanePos(int Lane) const;
	float GetMoveSpeed() const;

	static ALevelManager* GetLevelManager(const UObject* WorldContextObject);

protected:
	TDeque<AFloorTile*> CurrentFloorTiles;
	
	void SpawnRandomFloorTile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

};
