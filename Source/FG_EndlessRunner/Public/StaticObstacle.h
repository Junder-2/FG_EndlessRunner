// Fill out your copyright notice in the Description page of Project Settings.

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

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
};
