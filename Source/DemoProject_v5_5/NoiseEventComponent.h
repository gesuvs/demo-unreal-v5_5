#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseEventComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEMOPROJECT_V5_5_API UNoiseEventComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNoiseEventComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, FVector NormalImpulse,
						const FHitResult& Hit);

	// Volume base do ruído gerado por impacto
	UPROPERTY(EditAnywhere, Category = "Noise")
	float BaseNoiseVolume;

	// Volume máximo permitido para ruído
	UPROPERTY(EditAnywhere, Category = "Noise")
	float MaxNoiseVolume;

	// Velocidade mínima de impacto para gerar ruído
	UPROPERTY(EditAnywhere, Category = "Noise")
	float MinImpactVelocity;

	// Nome da tag que identifica objetos que podem gerar som
	UPROPERTY(EditAnywhere, Category = "Noise")
	FName NoiseTag;
};
