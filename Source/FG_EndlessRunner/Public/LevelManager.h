#pragma once

#include "CoreMinimal.h"
#include "Containers/Deque.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

class UBoxComponent;
class AFloorTile;
class ARunnerCharacter;
class URunnerSaveGame;

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Variables")
	float AddedObstaclesPerSecond = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game Variables")
	float BaseMoveSpeed = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game Variables")
	int BaseObstacleAmount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game Variables")
	float SpeedAcceleration = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game Variables")
	float PointsFromSpeed = .1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Game Variables")
	TArray<TSubclassOf<AFloorTile>> FloorTilesPrefabs;

public:
	UPROPERTY(Transient, BlueprintReadWrite, VisibleAnywhere, Category="Game Properties")
	float CurrentGameSpeed;

	UPROPERTY(Transient, BlueprintReadWrite, VisibleAnywhere, Category="Game Properties")
	float ObstacleDifficulty;
	
	ARunnerCharacter* GetRunnerCharacter() const;
	float GetRandomLanePos() const;
	float GetLanePos(int Lane) const;
	int GetLane(FVector Location) const;
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable)
	int GetCurrentScore() const;

	UFUNCTION(BlueprintCallable)
	int GetHighScore() const;
	
	static ALevelManager* GetLevelManager(const UObject* WorldContextObject);

protected:
	UPROPERTY(Transient)
	float CurrentAcceleration;
	
	UPROPERTY(Transient)
	float RawScore;

	UPROPERTY(Transient)
	int CurrentHighScore;

	void OnDamage();
	
	TDeque<AFloorTile*> CurrentFloorTiles;
	
	void SpawnRandomFloorTile();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

	UFUNCTION(BlueprintCallable)
	void SaveHighScore();

	UPROPERTY()
	TObjectPtr<URunnerSaveGame> SaveGame;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
