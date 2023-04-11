#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterCamera.generated.h"


UCLASS( ClassGroup=(Camera), meta=(BlueprintSpawnableComponent) )
class FG_ENDLESSRUNNER_API UCharacterCamera : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

public:	
	UCharacterCamera();

	void SetupAttachment(USceneComponent* InParent, FName InSocketName = NAME_None);

	UPROPERTY(Category= "CharacterCamera: Settings", EditAnywhere)
	bool bAutoCamera = true;

	UPROPERTY(Category= "CharacterCamera: Settings", EditAnywhere)
	FRotator RestingOffset = FRotator(-15.f, 0.f, 0.f);

	UPROPERTY(Category= "CharacterCamera: Settings", EditAnywhere)
	float IdleTime = 5.f;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Transient, VisibleAnywhere)
	float IdleTimer;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddLookInput(float Pitch, float Yaw);

	void SnapCamera();

		
};
