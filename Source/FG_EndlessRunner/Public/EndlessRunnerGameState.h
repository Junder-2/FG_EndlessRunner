#pragma once

#include "CoreMinimal.h"
#include "SaveGameInterface.h"
#include "Containers/Deque.h"
#include "GameFramework/GameStateBase.h"
#include "EndlessRunnerGameState.generated.h"

class AFloorTile;
class ARunnerCharacter;

UCLASS()
class FG_ENDLESSRUNNER_API AEndlessRunnerGameState : public AGameStateBase, public ISaveGameInterface
{
	GENERATED_BODY()

public:
	AEndlessRunnerGameState();

private:
	TStaticArray<TObjectPtr<ARunnerCharacter>, 2> PlayerCharacters; 

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
	UPROPERTY(Transient)
	int PlayerCount;
	
	UPROPERTY(Transient, BlueprintReadWrite, VisibleAnywhere, Category="Game Properties")
	float CurrentGameSpeed;

	UPROPERTY(Transient, BlueprintReadWrite, VisibleAnywhere, Category="Game Properties")
	float ObstacleDifficulty;
	
	ARunnerCharacter* GetRunnerCharacter(int PlayerIndex) const;
	float GetRandomLanePos() const;
	float GetLanePos(int Lane) const;
	int GetLane(FVector Location) const;
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable)
	int GetCurrentScore() const;

	UFUNCTION(BlueprintCallable)
	int GetHighScore() const;
	
protected:
	UPROPERTY(Transient)
	float CurrentAcceleration;
	
	UPROPERTY(Transient)
	float RawScore;

	UPROPERTY(Transient)
	int CurrentHighScore;

	UFUNCTION(BlueprintCallable)
	void SaveHighScore();

	void OnDamage(int PlayerIndex);
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath(int PlayerIndex);
	
	TDeque<AFloorTile*> CurrentFloorTiles;
	
	void SpawnRandomFloorTile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
};