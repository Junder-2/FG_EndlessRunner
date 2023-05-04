#include "RunnerCharacter.h"

#include "EndlessRunnerGameState.h"
#include "PlayerInputController.h"
#include "StaticObstacle.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ARunnerCharacter::ARunnerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	StoredLocationCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AfterImageCollision"));
	StoredLocationCollision->SetupAttachment(RootComponent);
	StoredLocationCollision->SetUsingAbsoluteLocation(true);
	StoredLocationCollision->InitCapsuleSize(42.f, 96.f);
	StoredLocationCollision->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	StoredLocationCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
	StoredLocationCollision->SetGenerateOverlapEvents(true);
}

void ARunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	StoredLocationCollision->SetCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	StoredLocationCollision->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnStoredLocationOverlap);
	
	GameState = Cast<AEndlessRunnerGameState>(UGameplayStatics::GetGameState(GetWorld()));
	
	PositionOffset = FVector::ZeroVector;

	if(APlayerInputController* MainController = Cast<APlayerInputController>(GetWorld()->GetFirstPlayerController()))
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			switch (PlayerController->GetLocalPlayer()->GetControllerId())
			{
				case 1:
					MainController->P1MoveEvent.AddUObject(this, &ARunnerCharacter::Move);
					MainController->P1JumpEvent.AddUObject(this, &ARunnerCharacter::JumpInput);

					if(GameState->IsTwoPlayerGame())
					{
						PositionOffset.X = 10;
					}
				break;

				case 2:
					MainController->P2MoveEvent.AddUObject(this, &ARunnerCharacter::Move);
					MainController->P2JumpEvent.AddUObject(this, &ARunnerCharacter::JumpInput);

					PositionOffset.X = -60;
				break;
			}
		}
	}

	if(GameState != nullptr)
	{
		CurrentLane = FMath::CeilToInt((GameState->NumOfLanes-1)/2.f);

		FVector StartPos = GetActorLocation();

		StartPos.Y = GameState->GetLanePos(CurrentLane);
		
		SetActorLocation(StartPos+PositionOffset);
	}
	
	BMovingLane = false;
	BIsJumping = false;
	TargetLane = CurrentLane;
	HitPoints = MaxHitPoints;
	InvincibleTimer = 0;
}

void ARunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(BMovingLane) MoveLane(DeltaTime);

	if(InvincibleTimer != 0)
	{
		InvincibleTimer -= DeltaTime;

		if(InvincibleTimer < 0) InvincibleTimer = 0;
	}

	if(StoredLocationTimer != 0)
	{
		StoredLocationTimer -= DeltaTime;

		if(StoredLocationTimer < 0)
		{
			StoredLocationCollision->SetWorldLocation(GetActorLocation());
			StoredLocationTimer = 0;
		}
		
	}	
}

void ARunnerCharacter::MoveLane(float DeltaTime)
{
	MoveLaneTimer += DeltaTime/MoveLaneTime;

	FVector Pos = GetActorLocation();

	if(MoveLaneTimer >= 1.f)
	{
		Pos.Y = TargetLaneY;
		
		BMovingLane = false;
		CurrentLane = TargetLane;

		StoredLocationTimer = StoredLocationDelay;

		if(int Value; !MoveInputQueue.IsEmpty() && MoveInputQueue.Dequeue(Value))
		{
			SwitchLane(Value);
		}
	}
	else Pos.Y = FMath::Lerp(StartLaneY, TargetLaneY, MoveLaneTimer);
	
	SetActorLocation(Pos);

}

void ARunnerCharacter::Move(const float Input)
{
	if(BMovingLane)
		MoveInputQueue.Enqueue(Input);
	else
		SwitchLane(Input);
}

void ARunnerCharacter::JumpInput()
{
	BIsJumping = true;
	Super::Jump();
}

void ARunnerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	BIsJumping = false;
}

void ARunnerCharacter::SwitchLane(const int Direction)
{
	const bool Increment = Direction > 0;
	const int NumOfLanes = GameState->NumOfLanes-1;
	
	if((CurrentLane <= 0 && !Increment) || (CurrentLane >= NumOfLanes && Increment) && Direction != 0) return;
	
	TargetLane = Increment ? TargetLane+1 : TargetLane-1;
	
	StartLaneY = GameState->GetLanePos(CurrentLane);
	TargetLaneY = GameState->GetLanePos(TargetLane);

	OnMoveLane(Increment);

	StoredLocationCollision->SetWorldLocation(GetActorLocation());
	StoredLocationTimer = 0;

	MoveLaneTimer = 0;
	BMovingLane = true;
}

void ARunnerCharacter::SwitchRandomLane()
{
	const int NumOfLanes = GameState->NumOfLanes-1;

	int Direction;
	
	if(CurrentLane >= NumOfLanes) Direction = -1;
	else if(CurrentLane <= 0) Direction = 1;
	else Direction = FMath::RandBool() ? -1 : 1;

	SwitchLane(Direction);
}

bool ARunnerCharacter::Damage(int Amount, const AActor* SourceActor)
{
	if(InvincibleTimer != 0 || HitPoints <= 0) return false;

	HitPoints -= Amount;
	InvincibleTimer = InvincibleTime;

	UE_LOG(LogTemp, Warning, TEXT("Output: %i"), HitPoints);
	OnDamage(InvincibleTime);
	OnDamageEvent.Broadcast();

	const int DamageLane = GameState->GetLane(SourceActor->GetActorLocation());

	UE_LOG(LogTemp, Warning, TEXT("Lane: %i"), DamageLane);
	
	if(BMovingLane && DamageLane == TargetLane) //Might move not super pretty placement
	{		
		MoveLaneTimer = 1-MoveLaneTimer;

		const int LastLane = CurrentLane;
		CurrentLane = TargetLane;
		TargetLane = LastLane;

		StartLaneY = GameState->GetLanePos(CurrentLane);
		TargetLaneY = GameState->GetLanePos(TargetLane);

		MoveInputQueue.Empty();
		OnMoveLane(CurrentLane < TargetLane);
		BMovingLane = true;
	}
	else if(!BMovingLane && DamageLane == CurrentLane)
	{
		MoveInputQueue.Empty();
		SwitchRandomLane();
	}

	if(HitPoints <= 0)
	{
		OnDeathEvent.Broadcast();
	}

	return true;
}

void ARunnerCharacter::SetMoveSpeed(const float Speed)
{
	CurrentMoveSpeed = Speed;
}

void ARunnerCharacter::OnStoredLocationOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(InvincibleTimer != 0) return;
	if(AStaticObstacle* Obstacle = Cast<AStaticObstacle>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("DodgedObstacle"));
		GameState->DestructRandomObstacle();
	}
}

void ARunnerCharacter::OnDamage_Implementation(float InvincibleDuration)
{
}

void ARunnerCharacter::OnMoveLane_Implementation(bool MoveRight)
{
}
