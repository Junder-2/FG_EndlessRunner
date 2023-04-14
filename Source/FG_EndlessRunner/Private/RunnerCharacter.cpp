// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
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
	}
}


void ARunnerCharacter::Move(const FInputActionValue& Value)
{
	const bool Increment = Value.Get<float>() > 0;

	if(BMovingLane || (CurrentLane <= 0 && !Increment) || (CurrentLane >= LevelManager->NumOfLanes-1 && Increment)) return;
	
	SwitchLane(Value.Get<float>());
}

void ARunnerCharacter::SwitchLane(const int Direction)
{
	if(Direction == 0)
	{
		if(CurrentLane >= LevelManager->NumOfLanes-1) TargetLane -= 1;
		else TargetLane += 1;
	}
	else TargetLane = Direction > 0 ? TargetLane+1 : TargetLane-1;
	
	StartLaneY = LevelManager->GetLanePos(CurrentLane);
	TargetLaneY = LevelManager->GetLanePos(TargetLane);

	OnMoveLane(Direction > 0);

	MoveLaneTimer = 0;
	BMovingLane = true;
}

void ARunnerCharacter::Jump(const FInputActionValue& Value)
{
	BIsJumping = true;
	Super::Jump();
}

void ARunnerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (CharacterCamera != nullptr)
	{
		CharacterCamera->AddLookInput(LookAxisVector.Y, LookAxisVector.X);
	}
}

void ARunnerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	BIsJumping = false;
}

void ARunnerCharacter::SetMoveSpeed(const float Speed)
{
	CurrentMoveSpeed = Speed;
}

bool ARunnerCharacter::Damage(int Amount, const AActor* SourceActor)
{
	if(InvincibleTimer != 0 || HitPoints <= 0) return false;

	HitPoints -= Amount;
	InvincibleTimer = InvincibleTime;

	UE_LOG(LogTemp, Warning, TEXT("Output: %i"), HitPoints);
	OnDamage(InvincibleTime);

	const int DamageLane = LevelManager->GetLane(SourceActor->GetActorLocation());

	UE_LOG(LogTemp, Warning, TEXT("Lane: %i"), DamageLane);
	
	if(BMovingLane && DamageLane == TargetLane)
	{
		MoveLaneTimer = 1-MoveLaneTimer;

		const int LastLane = CurrentLane;
		CurrentLane = TargetLane;
		TargetLane = LastLane;

		StartLaneY = LevelManager->GetLanePos(CurrentLane);
		TargetLaneY = LevelManager->GetLanePos(TargetLane);

		OnMoveLane(CurrentLane < TargetLane);
		BMovingLane = true;
	}
	else if(!BMovingLane && DamageLane == CurrentLane)
	{
		SwitchLane(0);
	}

	if(HitPoints <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Death"));
	}

	return true;
}

void ARunnerCharacter::OnDamage_Implementation(float InvincibleDuration)
{
	
}

void ARunnerCharacter::OnMoveLane_Implementation(bool MoveRight)
{
}
