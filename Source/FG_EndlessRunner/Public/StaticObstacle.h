#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StaticObstacle.generated.h"

class UBoxComponent;
UCLASS()
class FG_ENDLESSRUNNER_API AStaticObstacle : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CollisionBox;
	
public:	
	AStaticObstacle();

	FVector GetUpwardsOffset() const;
	float GetForwardOffset() const;

	void Destruct();

protected:	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnCollisionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};