#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterCharacter.h"
#include "DrawDebugHelpers.h"  // For debug drawing
#include "Perception/AISenseConfig_Hearing.h"

#include "MonsterAIController.generated.h"

UCLASS()
class DEMOPROJECT_V5_5_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIController();

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

public:
	/** Referência para o monstro controlado */
	AMonsterCharacter* ControlledMonster;

	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	float GetHearingRange() const;


	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
