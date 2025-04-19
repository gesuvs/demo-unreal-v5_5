// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoiseEmitterComponent.h"
#include "Components/ActorComponent.h"
#include "NoiseActor.generated.h"


UCLASS(Blueprintable, BlueprintType)
class DEMOPROJECT_V5_5_API ANoiseActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ANoiseActor();

protected:
	// Called when the game starts
	 void BeginPlay() override;

public:
	// Called every frame
	 void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNoiseEmitterComponent* NoiseEmitter;
};
