#include "CharacterCamera.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values for this component's properties
UCharacterCamera::UCharacterCamera()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->TargetArmLength = 500.f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bAutoCamera = true;
	RestingOffset = FRotator(-15.f, 0.f, 0.f);
	IdleTime = 2.f;
	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void UCharacterCamera::SetupAttachment(USceneComponent* InParent, FName InSocketName)
{
	CameraBoom->SetupAttachment(InParent, InSocketName);
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}


// Called when the game starts
void UCharacterCamera::BeginPlay()
{
	Super::BeginPlay();

	IdleTimer = IdleTime;	
}


// Called every frame
void UCharacterCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(IdleTimer >= IdleTime || bAutoCamera)
	{
		if (const APawn* OwningPawn = Cast<APawn>(GetOwner()))
		{
			FRotator CurrentRotation = OwningPawn->GetControlRotation();
			const FRotator Rotation = GetOwner()->GetActorRotation();
			const FRotator YawRotation(Rotation.Pitch+RestingOffset.Pitch,
									   Rotation.Yaw+RestingOffset.Yaw, RestingOffset.Roll);

			CurrentRotation = FMath::RInterpTo(CurrentRotation, YawRotation, DeltaTime, 5.f);
			
			OwningPawn->GetController()->SetControlRotation(CurrentRotation);
		}
	}
	else
	{
		IdleTimer += DeltaTime;
	}
}

void UCharacterCamera::AddLookInput(float Pitch, float Yaw)
{
	if(bAutoCamera) return;
	
	if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
	{
		OwningPawn->AddControllerYawInput(Yaw);
		OwningPawn->AddControllerPitchInput(Pitch);

		IdleTimer = 0;
	}
}

void UCharacterCamera::SnapCamera()
{
	IdleTimer = IdleTime;
}

