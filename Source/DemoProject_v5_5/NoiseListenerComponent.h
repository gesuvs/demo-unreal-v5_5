// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NotificationManager.h"
#include "GameFramework/Character.h"  // For ACharacter

#include "NoiseListenerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNoiseHeardSignature, const FVector&, NoiseLocation, float, Volume);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEMOPROJECT_V5_5_API UNoiseListenerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNoiseListenerComponent();


	virtual void BeginPlay() override;

	void TickComponent(float DeltaTime, ELevelTick TickType,
	                   FActorComponentTickFunction* ThisTickFunction) override;

	/** Raio máximo de audição */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float HearingRadius;

	/** Volume mínimo para escutar o som */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float MinimumVolumeThreshold;

	/** Intervalo entre verificações de ruídos */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float CheckInterval;

	/** Tempo recente para considerar sons válidos */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
	float RecentTimeWindow;

	/** Delegate para eventos de barulho ouvido */
	UPROPERTY(BlueprintAssignable, Category = "Noise")
	FOnNoiseHeardSignature OnNoiseHeard;

	// Adicione esta propriedade
	UPROPERTY()
	ANotificationManager* NotificationManagerRef;

	// Adicione este método
	void ValidateNotificationManager();

	// Adicione para debug
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = false;

	UFUNCTION()
	void HandleNoiseHeard(const FVector& NoiseLocation, float Volume);



private:
	float TimeSinceLastCheck;
};
