#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UPlayerViewTrackerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEMOPROJECT_V5_5_API UPlayerViewTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerViewTrackerComponent();

	/** Verifica se o ponto está dentro do campo de visão atual do player */
	bool IsLocationInView(const FVector& Location, float& OutDotProduct) const;

	/** Verifica se um ator está no campo de visão */
	bool IsActorInView(AActor* TargetActor, float& OutDotProduct) const;

	/** Campo de visão em graus (ex: 90) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="View Tracking")
	float FieldOfViewAngleDegrees = 90.0f;

	/** Distância máxima para considerar algo visível */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="View Tracking")
	float MaxViewDistance = 4000.0f;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Atualiza internamente a direção de visão */
	void UpdateViewData();

	FVector LastViewLocation;
	FVector LastViewDirection;
};
