#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FG_EndlessRunnerGameModeBase.generated.h"

class ARunnerCharacter;

UCLASS()
class FG_ENDLESSRUNNER_API AFG_EndlessRunnerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<ARunnerCharacter> P1_Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<ARunnerCharacter> P2_Character;

public:
	virtual void StartPlay() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
};