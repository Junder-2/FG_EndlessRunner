// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	GameMode = Cast<AFG_EndlessRunnerGameModeBase>(GetWorld()->GetAuthGameMode());

	if(GameMode != nullptr)
	{
		CurrentLane = FMath::CeilToInt((GameMode->NumOfLanes-1)/2.f);

		FVector StartPos = GetActorLocation();

		StartPos.Y = GameMode->GetLanePos(CurrentLane);
		
		SetActorLocation(StartPos);
	}

	BMovingLane = false;

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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARunnerCharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ARunnerCharacter::Jump);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARunnerCharacter::Look);
	}
}

void ARunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		GetCharacterMovement()->AddInputVector(ForwardDirection);
	}

	if(BMovingLane) MoveLane(DeltaTime);
}

void ARunnerCharacter::MoveLane(float DeltaTime)
{
	MoveLaneTimer += DeltaTime/MoveLaneTime;

	FVector Pos = GetActorLocation();

	Pos.Y = FMath::Lerp(StartLaneY, TargetLaneY, MoveLaneTimer);
		
	SetActorLocation(Pos, true);

	if(MoveLaneTimer >= 1.f)
	{
		BMovingLane = false;
		CurrentLane = TargetLane;
	}
}

void ARunnerCharacter::Move(const FInputActionValue& Value)
{
	const bool Increment = Value.Get<float>() > 0;

	if(BMovingLane || (CurrentLane <= 0 && !Increment) || (CurrentLane >= GameMode->NumOfLanes-1 && Increment)) return;
	
	TargetLane = Increment ? TargetLane+1 : TargetLane-1;
	StartLaneY = GetActorLocation().Y;
	TargetLaneY = GameMode->GetLanePos(TargetLane);

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

void ARunnerCharacter::SetMoveSpeedFromMultiplier(float Multiplier)
{
	GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed*Multiplier;
}

void ARunnerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	BIsJumping = false;
}
