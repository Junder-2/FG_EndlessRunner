#include "FG_EndlessRunnerGameModeBase.h"

#include "RunnerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

UClass* AFG_EndlessRunnerGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if(P1_Character && P2_Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerID: %i"), GetNumPlayers());
		
		const int ControllerId = GetNumPlayers();

		if(ControllerId == 2) return P1_Character;
		if(ControllerId == 3) return P2_Character;
	}
	
	return nullptr;
}

void AFG_EndlessRunnerGameModeBase::StartPlay()
{
	PlayerControllerClass = APlayerController::StaticClass();
	UGameplayStatics::CreatePlayer(GetWorld(), 1, true);
	UGameplayStatics::CreatePlayer(GetWorld(), 2, true);
	Super::StartPlay();
}

