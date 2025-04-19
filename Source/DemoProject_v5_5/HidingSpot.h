// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HidingSpot.generated.h"

UENUM(BlueprintType)
enum class EHidingSpotType : uint8
{
	UnderBed UMETA(DisplayName = "Under Bed"),
	BehindDoor UMETA(DisplayName = "Behind Door"),
	InCloset UMETA(DisplayName = "In Closet"),
	InShadow UMETA(DisplayName = "In Shadow")
};

USTRUCT(BlueprintType)
struct FSpotData
{
	GENERATED_BODY()

	// Localização do esconderijo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Esconderijo")
	FVector Location;

	// Capacidade do esconderijo (quantos monstros ele pode acomodar)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Esconderijo")
	int32 Capacity;

	// Quantidade de monstros que estão atualmente no esconderijo
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Esconderijo")
	int32 CurrentOccupancy;

	// Verifica se o esconderijo tem capacidade
	bool CanAccommodate(int32 NumMonsters) const
	{
		return (CurrentOccupancy + NumMonsters <= Capacity);
	}

	// Aumenta a ocupação quando um monstro se esconde
	void AddMonster(int32 NumMonsters)
	{
		CurrentOccupancy += NumMonsters;
	}

	// Diminui a ocupação quando um monstro sai do esconderijo
	void RemoveMonster(int32 NumMonsters)
	{
		CurrentOccupancy -= NumMonsters;
	}
};


UCLASS()
class DEMOPROJECT_V5_5_API AHidingSpot : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHidingSpot();

	// Dados do esconderijo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hiding")
	FSpotData SpotData;


	// Função para verificar se o esconderijo está disponível
	bool IsAvailable(int32  NumMonsters) const;

	// Tipo de esconderijo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hiding")
	EHidingSpotType SpotType;

	// Capacidade máxima de tamanho do monstro que pode se esconder aqui
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hiding")
	float MaxCreatureSize;

	// Posição exata que o monstro deve ocupar para se esconder
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hiding")
	FVector HidingPosition;

	// Define se o esconderijo está ocupado
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hiding")
	bool bIsOccupied;

	// Checa se o monstro pode se esconder aqui baseado no tamanho dele
	UFUNCTION(BlueprintCallable)
	bool CanBeUsedBy(ECreatureSize CreatureSize) const;

	// Ocupa o esconderijo
	void Occupy();

	// Libera o esconderijo
	void Vacate();
	
};
