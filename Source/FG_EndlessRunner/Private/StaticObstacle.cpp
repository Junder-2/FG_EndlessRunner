#include "StaticObstacle.h"

#include "RunnerCharacter.h"
#include "Components/BoxComponent.h"

AStaticObstacle::AStaticObstacle()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionBox->InitBoxExtent(FVector(50.f, 50.f, 50.f));
	CollisionBox->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	
	RootComponent = CollisionBox;
}

void AStaticObstacle::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AStaticObstacle::OnCollisionOverlap);
}

FVector AStaticObstacle::GetUpwardsOffset() const
{
	return (CollisionBox->GetScaledBoxExtent().Z*.5f)*FVector::UpVector;
}

float AStaticObstacle::GetForwardOffset() const
{
	return (CollisionBox->GetScaledBoxExtent().X*.5f);
}

void AStaticObstacle::Destruct()
{
	Destroy();
}

void AStaticObstacle::OnCollisionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherComp->ComponentHasTag(TEXT("Ignore"))) return;
	
	if(ARunnerCharacter* Player = Cast<ARunnerCharacter>(OtherActor))
	{
		if(Player->Damage(1, this))
		{
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}
