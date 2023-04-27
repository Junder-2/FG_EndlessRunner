#include "RunnerCharacter.h"

#include "CharacterCamera.h"
#include "EnhancedInputComponent.h"
#include "LevelManager.h"
#include "PlayerInputController.h"
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

	CharacterCamera = CreateDefaultSubobject<UCharacterCamera>(TEXT("OrbitCamera"));
	CharacterCamera->SetupAttachment(RootComponent);
}

void ARunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	LevelManager = ALevelManager::GetLevelManager(GetWorld());

	if(LevelManager != nullptr)
	{
		CurrentLane = FMath::CeilToInt((LevelManager->NumOfLanes-1)/2.f);

		FVector StartPos = GetActorLocation();

		StartPos.Y = LevelManager->GetLanePos(CurrentLane);
		
		SetActorLocation(StartPos);
	}

	BMovingLane = false;
	BIsJumping = false;
	TargetLane = CurrentLane;
	HitPoints = MaxHitPoints;
	InvincibleTimer = 0;

	if(APlayerInputController* MainController = Cast<APlayerInputController>(GetWorld()->GetFirstPlayerController()))
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			switch (PlayerController->GetLocalPlayer()->GetControllerId())
			{
				case 1:
					MainController->P1MoveEvent.AddUObject(this, &ARunnerCharacter::Move);
					MainController->P1JumpEvent.AddUObject(this, &ARunnerCharacter::JumpInput);
				break;

				case 2:
					MainController->P2MoveEvent.AddUObject(this, &ARunnerCharacter::Move);
				MainController->P2JumpEvent.AddUObject(this, &ARunnerCharacter::JumpInput);
				break;
			}
		}
	}
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

void ARunnerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (CharacterCamera != nullptr)
	{
		CharacterCamera->AddLookInput(LookAxisVector.Y, LookAxisVector.X);
	}
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
	const int NumOfLanes = LevelManager->NumOfLanes-1;
	
	if((CurrentLane <= 0 && !Increment) || (CurrentLane >= NumOfLanes && Increment) && Direction != 0) return;
	
	TargetLane = Increment ? TargetLane+1 : TargetLane-1;
	
	StartLaneY = LevelManager->GetLanePos(CurrentLane);
	TargetLaneY = LevelManager->GetLanePos(TargetLane);

	OnMoveLane(Increment);

	MoveLaneTimer = 0;
	BMovingLane = true;
}

void ARunnerCharacter::SwitchRandomLane()
{
	const int NumOfLanes = LevelManager->NumOfLanes-1;

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

	const int DamageLane = LevelManager->GetLane(SourceActor->GetActorLocation());

	UE_LOG(LogTemp, Warning, TEXT("Lane: %i"), DamageLane);
	
	if(BMovingLane && DamageLane == TargetLane) //Might move not super pretty placement
	{		
		MoveLaneTimer = 1-MoveLaneTimer;

		const int LastLane = CurrentLane;
		CurrentLane = TargetLane;
		TargetLane = LastLane;

		StartLaneY = LevelManager->GetLanePos(CurrentLane);
		TargetLaneY = LevelManager->GetLanePos(TargetLane);

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

void ARunnerCharacter::OnDamage_Implementation(float InvincibleDuration)
{
}

void ARunnerCharacter::OnMoveLane_Implementation(bool MoveRight)
{
}