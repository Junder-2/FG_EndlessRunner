#pragma once

#include "CoreMinimal.h"
#include "CharacterCamera.h"
#include "InputAction.h"
#include "FG_EndlessRunner/FG_EndlessRunnerGameModeBase.h"
#include "GameFramework/Character.h"
#include "RunnerCharacter.generated.h"

UCLASS()
class FG_ENDLESSRUNNER_API ARunnerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterCamera> CharacterCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

public:
	ARunnerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner Settings")
	float BaseMoveSpeed = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner Settings")
	float MoveLaneTime = .25f;

	void SetMoveSpeedFromMultiplier(float Multiplier);

	UFUNCTION(BlueprintCallable, Category=Character)
	bool GetIsJumping() const {return BIsJumping;}

protected:
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);

	UPROPERTY(Transient)
	bool BIsJumping;

	void MoveLane(float DeltaTime);

	TObjectPtr<AFG_EndlessRunnerGameModeBase> GameMode;

	UPROPERTY(Transient, VisibleAnywhere, Category="Runner Debug")
	bool BMovingLane;
	UPROPERTY(Transient, VisibleAnywhere, Category="Runner Debug")
	float MoveLaneTimer;

	UPROPERTY(Transient, VisibleAnywhere, Category="Runner Debug")
	int CurrentLane;
	UPROPERTY(Transient)
	int TargetLane;

	UPROPERTY(Transient)
	float StartLaneY;
	UPROPERTY(Transient)
	float TargetLaneY;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;
};
