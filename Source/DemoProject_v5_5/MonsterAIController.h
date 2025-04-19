#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterCharacter.h"
#include "DrawDebugHelpers.h"  // For debug drawing

#include "MonsterAIController.generated.h"

class ANotificationManager;
class AHidingSpot;
class AMonsterCharacter;

UCLASS()
class DEMOPROJECT_V5_5_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIController();

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnNoiseHeard(FVector NoiseLocation, float NoiseVolume);

public:
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = false;


	/** Referência para o monstro controlado */
	AMonsterCharacter* ControlledMonster;

	/** Referência para o NotificationManager no mundo */
	ANotificationManager* NotificationManager;

	/** Distância máxima para escutar barulhos */
	static constexpr float NoiseDetectionRadius = 3000.0f;

	/** Tempo limite para considerar um barulho recente */
	static constexpr float MaxNoiseAge = 5.0f;

	/** Distância mínima de fuga ao se afastar do player */
	static constexpr float MinEscapeDistance = 2000.0f;

	/** Atualiza lógica com base nos barulhos escutados */
	void ProcessHeardNoises();

	/** Procura o melhor esconderijo possível */
	AHidingSpot* FindBestHidingSpot(ECreatureSize MonsterSize) const;

	/** Comportamento de fuga do monstro */
	void FleeAndHide();

	/** Referência ao último esconderijo utilizado */
	AHidingSpot* CurrentHidingSpot;

	FTimerHandle ReactionTimerHandle;
	FVector LastHeardNoiseLocation;
	float LastHeardNoiseVolume;

	bool CanHearNoise(float NoiseVolume, float Distance) const;

	// Propriedades UPROPERTY para aparecer no Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Hearing")
	float HearingRadius = 3000.0f; // Alcance máximo de audição em unidades (cm)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Hearing", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float NoiseSensitivity = 0.7f; // Sensibilidade (0.0 a 1.0)


	// Configurações de movimento
	UPROPERTY(EditDefaultsOnly, Category = "AI|Movement")
	float AcceptanceRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Movement")
	float NoiseInvestigationDuration = 5.f;

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	void MoveToNoiseLocation(const FVector& NoiseLocation);

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	void StopNoiseInvestigation();

	// Função auxiliar para calcular alcance efetivo
	float GetEffectiveHearingRange(float NoiseVolume) const
	{
		return HearingRadius * NoiseSensitivity * NoiseVolume;
	}

	FVector CurrentNoiseLocation;

protected:
	FTimerHandle NoiseInvestigationTimer;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
};
