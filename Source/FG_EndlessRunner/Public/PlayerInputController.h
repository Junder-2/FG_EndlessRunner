#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerInputController.generated.h"

DECLARE_EVENT_OneParam(APlayerInputController, FInputMove, float)

DECLARE_EVENT(APlayerInputController, FInputJump)

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class FG_ENDLESSRUNNER_API APlayerInputController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpActionP1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveActionP1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpActionP2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveActionP2;

public:
	FInputMove P1MoveEvent;
	FInputMove P2MoveEvent;
	
	FInputJump P1JumpEvent;
	FInputJump P2JumpEvent;

protected:
	void MoveP1(const FInputActionValue& Value);
	void JumpP1(const FInputActionValue& Value);
	void MoveP2(const FInputActionValue& Value);
	void JumpP2(const FInputActionValue& Value);

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void SpawnPlayerCameraManager() override;
	virtual ACameraActor* GetAutoActivateCameraForPlayer() const override;
};
