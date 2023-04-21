#include "StaticObstacle.h"

#include "RunnerCharacter.h"
#include "Components/BoxComponent.h"

AStaticObstacle::AStaticObstacle()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionBox->InitBoxExtent(FVector(50.f, 50.f, 50.f));
	CollisionBox->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	
	RootComponent = CollisionBox;
}

FVector AStaticObstacle::GetUpwardsOffset() const
{
	return (CollisionBox->GetScaledBoxExtent().Z*.5f)*FVector::UpVector;
}

float AStaticObstacle::GetForwardOffset() const
{
	return (CollisionBox->GetScaledBoxExtent().X*.5f);
}

void AStaticObstacle::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if(ARunnerCharacter* Player = Cast<ARunnerCharacter>(OtherActor))
	{
		if(Player->Damage(1, this))
		{
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

