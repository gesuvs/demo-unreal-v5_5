// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HidingSpot.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"


// UENUM(BlueprintType)
// enum class EMonsterState : uint8
// {
// 	Idle UMETA(DisplayName = "Idle"),
// 	Hunting UMETA(DisplayName = "Hunting"),
// 	Fleeing UMETA(DisplayName = "Fleeing"),
// };


UCLASS()
class DEMOPROJECT_V5_5_API AMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonster();

	// Duração do som que o monstro consegue ouvir
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monstro|Sons")
	float MaxSoundRange;

	// Sensibilidade do monstro ao som
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monstro|Sons")
	float SoundSensitivity;

	// Se o monstro é agressivo e atacará o jogador
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monstro|Comportamento")
	bool bIsAggressive;

	// A chance do monstro tentar se esconder
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monstro|Comportamento")
	float HideChance;

	// Indica se o monstro tenta fugir ao ser atacado
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monstro|Comportamento")
	bool bCanFlee;

	// A distância mínima para o monstro fugir
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monstro|Comportamento")
	float MinFleeDistance;

	// Lista de esconderijos disponíveis
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monstro|Esconderijos")
	TArray<AHidingSpot*> HidingSpots;

	// Ponto de esconderijo do monstro
	FVector HidingSpot;

	// Método para ser chamado quando o monstro escuta um som
	virtual void HearSound(FVector SoundLocation, float SoundVolume);

	// Método para o monstro se esconder
	virtual void Hide();

	// Método para o monstro atacar
	virtual void AttackPlayer();

	// Método para o monstro fugir
	virtual void Flee();

	// Método de atualização do monstro
	virtual void Tick(float DeltaTime) override;

	// Definir se o monstro está indo para o esconderijo
	bool bIsHiding;

	// Método para buscar um esconderijo disponível
	AHidingSpot* FindAvailableHidingSpot();
};
