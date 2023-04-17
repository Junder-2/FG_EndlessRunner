// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Deque.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

class UBoxComponent;
class AFloorTile;
class ARunnerCharacter;

UCLASS()
class FG_ENDLESSRUNNER_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game Variables")
	float SpeedAcceleration = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game Variables")
	float PointsFromSpeed = .1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Game Variables")
	TArray<TSubclassOf<AFloorTile>> FloorTilesPrefabs;

public:
	UPROPERTY(Transient, BlueprintReadWrite, VisibleAnywhere, Category="Game Properties")
	float CurrentGameSpeed;
	
	ARunnerCharacter* GetRunnerCharacter() const;
	float GetRandomLanePos() const;
	float GetLanePos(int Lane) const;
	int GetLane(FVector Location) const;
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable)
	int GetCurrentScore() const;
	
	static ALevelManager* GetLevelManager(const UObject* WorldContextObject);

protected:
	UPROPERTY(Transient)
	float CurrentAcceleration;
	
	UPROPERTY(Transient)
	float RawScore;

	void OnDamage();
	
	TDeque<AFloorTile*> CurrentFloorTiles;
	
	void SpawnRandomFloorTile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
