// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerCharacter.h"

#include "CharacterCamera.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "LevelManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
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

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &ARunnerCharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARunnerCharacter::Jump);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARunnerCharacter::Look);
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

	Pos.Y = FMath::Lerp(StartLaneY, TargetLaneY, MoveLaneTimer);
	
	SetActorLocation(Pos);

	if(MoveLaneTimer >= 1.f)
	{
		BMovingLane = false;
		CurrentLane = TargetLane;

		int Value;
		if(!MoveInputQueue.IsEmpty() && MoveInputQueue.Dequeue(Value))
		{
			SwitchLane(Value);
		}
	}
}

void ARunnerCharacter::Move(const FInputActionValue& Value)
{
	if(BMovingLane)
		MoveInputQueue.Enqueue(Value.Get<float>());
	else
		SwitchLane(Value.Get<float>());
}

void ARunnerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (CharacterCamera != nullptr)
	{
		CharacterCamera->AddLookInput(LookAxisVector.Y, LookAxisVector.X);
	}
}

void ARunnerCharacter::Jump(const FInputActionValue& Value)
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
		UE_LOG(LogTemp, Warning, TEXT("Death"));
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
