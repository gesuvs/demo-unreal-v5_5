#include "NoiseEventComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SoundManager.h"

static const float DEFAULT_BASE_NOISE_VOLUME = 20.0f;
static const float DEFAULT_MAX_NOISE_VOLUME = 1000.0f;
static const float DEFAULT_MIN_IMPACT_VELOCITY = 100.0f;
static const FName DEFAULT_NOISE_TAG = TEXT("PhysicsNoise");

UNoiseEventComponent::UNoiseEventComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	BaseNoiseVolume = DEFAULT_BASE_NOISE_VOLUME;
	MaxNoiseVolume = DEFAULT_MAX_NOISE_VOLUME;
	MinImpactVelocity = DEFAULT_MIN_IMPACT_VELOCITY;
	NoiseTag = DEFAULT_NOISE_TAG;
}

void UNoiseEventComponent::BeginPlay()
{
	Super::BeginPlay();

	// Se o ator possui componente de física, conectamos o evento de impacto
	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(
		GetOwner()->GetComponentByClass(UPrimitiveComponent::StaticClass())))
	{
		if (PrimComp->IsSimulatingPhysics())
		{
			PrimComp->OnComponentHit.AddDynamic(this, &UNoiseEventComponent::OnComponentHit) ;
		}
	}
}

void UNoiseEventComponent::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                          const FHitResult& Hit)
{
	const float ImpactVelocity = NormalImpulse.Size();

	// Se o impacto não for suficientemente forte, ignoramos
	if (ImpactVelocity < MinImpactVelocity)
		return;

	// Calcula o volume com base na intensidade do impacto e limita ao volume máximo
	const float CalculatedVolume = FMath::Clamp(ImpactVelocity * BaseNoiseVolume, 0.0f, MaxNoiseVolume);

	// Obtém o SoundManager e registra o ruído
	if (UWorld* World = GetWorld())
	{
		ASoundManager* Manager = Cast<ASoundManager>(
			UGameplayStatics::GetActorOfClass(World, ASoundManager::StaticClass()));
		if (Manager)
		{
			Manager->RegisterNoise(GetOwner()->GetActorLocation(), CalculatedVolume);
		}
	}

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("[NoiseEventComponent] Som gerado! Volume: %.2f | Velocidade: %.2f"), CalculatedVolume,
	       ImpactVelocity);
#endif
}
