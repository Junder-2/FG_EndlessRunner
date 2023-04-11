// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FG_EndlessRunnerGameModeBase.generated.h"

class ARunnerCharacter;

UCLASS()
class FG_ENDLESSRUNNER_API AFG_EndlessRunnerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFG_EndlessRunnerGameModeBase();

private:
	TObjectPtr<ARunnerCharacter> RunnerCharacter; 

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Variables")
	int NumOfLanes = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Variables")
	float TotalWidth = 250.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Game Variables")
	float AddedSpeedPerSecond = 0.1f;

public:
	UPROPERTY(Transient, BlueprintReadWrite, VisibleAnywhere, Category="Game Properties")
	float CurrentGameSpeed;
	
	ARunnerCharacter* GetRunnerCharacter() const;
	float GetLanePos(int Lane);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
};
