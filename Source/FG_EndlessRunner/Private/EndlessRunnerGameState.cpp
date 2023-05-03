#include "EndlessRunnerGameState.h"

#include "FloorTile.h"
#include "RunnerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AEndlessRunnerGameState::AEndlessRunnerGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEndlessRunnerGameState::BeginPlay()
{
	Super::BeginPlay();

	PlayerCount = UGameplayStatics::GetNumPlayerControllers(GetWorld());

	for (int i = 0; i < FMath::Min(PlayerCount-1, PlayerCharacters.Num()); i++)
	{
		PlayerCharacters[i] = Cast<ARunnerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), i+1));
		PlayerCharacters[i]->OnDamageEvent.AddUObject(this, &AEndlessRunnerGameState::OnDamage, i);
		PlayerCharacters[i]->OnDeathEvent.AddUObject(this, &AEndlessRunnerGameState::OnDeath, i);
		UE_LOG(LogTemp, Warning, TEXT("Found Player %i"), i);
	}

	CurrentHighScore = LoadHighScore();

	CurrentGameSpeed = 1.f;
	ObstacleDifficulty = BaseObstacleAmount;

	for (int i = 0; i < 10; i++)
	{
		SpawnRandomFloorTile();
	}
}

void AEndlessRunnerGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(CurrentAcceleration != 1)
	{
		CurrentAcceleration += DeltaSeconds*SpeedAcceleration;

		if(CurrentAcceleration > 1.f) CurrentAcceleration = 1;
	}

	CurrentGameSpeed += DeltaSeconds*CurrentAcceleration;
	ObstacleDifficulty += DeltaSeconds*AddedObstaclesPerSecond*FMath::Max(CurrentAcceleration, 0);

	RawScore += DeltaSeconds*CurrentGameSpeed*PointsFromSpeed*FMath::Max(CurrentAcceleration, 0);

	if(RawScore > CurrentHighScore)
	{
		CurrentHighScore = RawScore;
	}

	for (TObjectPtr<ARunnerCharacter> Player : PlayerCharacters)
	{
		if(Player != nullptr)
		{
			Player->SetMoveSpeed(GetMoveSpeed());
		}
	}	

	if(!CurrentFloorTiles.IsEmpty())
	{
		if(CurrentFloorTiles.First()->GetActorLocation().X < -5000.f)
		{
			TArray<AActor*> Temp;
			CurrentFloorTiles.First()->GetAttachedActors(Temp);
			
			for (AActor* CActor : Temp)
			{
				CActor->Destroy();
			}
			CurrentFloorTiles.First()->Destroy();
			
			CurrentFloorTiles.PopFirst();
			SpawnRandomFloorTile();
		}
	}
}

void AEndlessRunnerGameState::SpawnRandomFloorTile()
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

void AEndlessRunnerGameState::OnDamage(int PlayerIndex)
{
	CurrentAcceleration = -.5;
}

ARunnerCharacter* AEndlessRunnerGameState::GetRunnerCharacter(const int PlayerIndex) const
{
	return PlayerCharacters[PlayerIndex];
}

float AEndlessRunnerGameState::GetRandomLanePos() const
{
	return GetLanePos(FMath::RandRange(0, NumOfLanes-1));
}

float AEndlessRunnerGameState::GetLanePos(const int Lane) const
{
	return FMath::Lerp(-TotalWidth*.5f, TotalWidth*.5f, (float)Lane/(NumOfLanes-1));
}

int AEndlessRunnerGameState::GetLane(const FVector Location) const
{
	return UKismetMathLibrary::NormalizeToRange(Location.Y, -TotalWidth*.5f, TotalWidth*.5f) * (NumOfLanes-1);
}

float AEndlessRunnerGameState::GetMoveSpeed() const
{
	return BaseMoveSpeed + CurrentGameSpeed*AddedSpeedPerSecond;
}

int AEndlessRunnerGameState::GetCurrentScore() const
{
	return RawScore;
}

int AEndlessRunnerGameState::GetHighScore() const
{
	return CurrentHighScore;	
}

void AEndlessRunnerGameState::SaveHighScore()
{
	SaveNewHighScore(CurrentHighScore);
}