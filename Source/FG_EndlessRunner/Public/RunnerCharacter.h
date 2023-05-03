#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RunnerCharacter.generated.h"

class AEndlessRunnerGameState;
DECLARE_EVENT(ARunnerCharacter, FRunnerDamage)

DECLARE_EVENT(ARunnerCharacter, FRunnerDeath)

class UCharacterCamera;

UCLASS()
class FG_ENDLESSRUNNER_API ARunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARunnerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner Settings")
	float MoveLaneTime = .25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner Settings")
	float InvincibleTime = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner Settings")
	int MaxHitPoints = 3;

	UFUNCTION(BlueprintCallable, Category=Character)
	bool GetIsJumping() const { return BIsJumping; }
	
	UFUNCTION(BlueprintCallable, Category=Character)
	int GetHitPoints() const { return HitPoints; }	

	UFUNCTION(BlueprintCallable, Category=Character)
	float GetMoveSpeed() const { return CurrentMoveSpeed; }
	void SetMoveSpeed(float Speed);

	bool Damage(int Amount, const AActor* SourceActor);
	
	UFUNCTION(BlueprintNativeEvent, Category=Character)
	void OnDamage(float InvincibleDuration);
	virtual void OnDamage_Implementation(float InvincibleDuration);
	FRunnerDamage OnDamageEvent;

	FRunnerDeath OnDeathEvent;

	UFUNCTION(BlueprintNativeEvent, Category=Character)
	void OnMoveLane(bool MoveRight);
	virtual void OnMoveLane_Implementation(bool MoveRight);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void Move(float Input);
	TQueue<int> MoveInputQueue;
	UFUNCTION()
	void JumpInput();

	UPROPERTY(Transient)
	bool BIsJumping;

	void MoveLane(float DeltaTime);

	void SwitchLane(int Direction);

	void SwitchRandomLane();
	
	TObjectPtr<AEndlessRunnerGameState> GameState;

	UPROPERTY(Transient)
	float InvincibleTimer;
	UPROPERTY(Transient, VisibleAnywhere, Category="Runner Debug")
	int HitPoints;

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
	UPROPERTY(Transient)
	float CurrentMoveSpeed;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void Landed(const FHitResult& Hit) override;
};