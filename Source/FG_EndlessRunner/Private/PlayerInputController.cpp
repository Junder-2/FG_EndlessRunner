#include "PlayerInputController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void APlayerInputController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void APlayerInputController::SpawnPlayerCameraManager()
{
	
}

ACameraActor* APlayerInputController::GetAutoActivateCameraForPlayer() const
{
	return nullptr;
}

void APlayerInputController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{		
		EnhancedInputComponent->BindAction(MoveActionP1, ETriggerEvent::Started, this, &APlayerInputController::MoveP1);
		EnhancedInputComponent->BindAction(JumpActionP1, ETriggerEvent::Started, this, &APlayerInputController::JumpP1);

		EnhancedInputComponent->BindAction(MoveActionP2, ETriggerEvent::Started, this, &APlayerInputController::MoveP2);
		EnhancedInputComponent->BindAction(JumpActionP2, ETriggerEvent::Started, this, &APlayerInputController::JumpP2);
	}
}

void APlayerInputController::MoveP1(const FInputActionValue& Value)
{
	P1MoveEvent.Broadcast(Value.Get<float>());
}

void APlayerInputController::JumpP1(const FInputActionValue& Value)
{
	P1JumpEvent.Broadcast();
}

void APlayerInputController::MoveP2(const FInputActionValue& Value)
{
	P2MoveEvent.Broadcast(Value.Get<float>());
}

void APlayerInputController::JumpP2(const FInputActionValue& Value)
{
	P2JumpEvent.Broadcast();
}