// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseListenerComponent.h"

#include "SoundManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h" // For debug drawing
#include "MonsterAIController.h"
#include "GameFramework/Pawn.h"

#include "Kismet/KismetSystemLibrary.h" // For math utilities


// Sets default values for this component's properties
UNoiseListenerComponent::UNoiseListenerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	HearingRadius = 2000.0f;
	MinimumVolumeThreshold = 50.0f;
	CheckInterval = 0.5f;
	RecentTimeWindow = 5.0f;

	TimeSinceLastCheck = 0.0f;

	OnNoiseHeard.AddDynamic(this, &UNoiseListenerComponent::HandleNoiseHeard);
}


// Called when the game starts
void UNoiseListenerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Bind the delegate
	OnNoiseHeard.AddDynamic(this, &UNoiseListenerComponent::HandleNoiseHeard);


	// Verificação inicial do SoundManager
	ValidateSoundManager();
}


// Called every frame
void UNoiseListenerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !GetWorld())
	{
		return;
	}


	TimeSinceLastCheck += DeltaTime;

	if (TimeSinceLastCheck < CheckInterval)
	{
		return;
	}

	TimeSinceLastCheck = 0.0f;

	// Verificação segura do SoundManager
	if (!SoundManagerRef || !SoundManagerRef->IsValidLowLevel())
	{
		ValidateSoundManager();
		if (!SoundManagerRef) return;
	}

	// ASoundManager* Manager = Cast<ASoundManager>(
	//     UGameplayStatics::GetActorOfClass(GetWorld(), ASoundManager::StaticClass()));
	//
	//
	// if (!Manager)
	//     return;

	// Verificação do owner
	AActor* Owner = GetOwner();
	if (!Owner) return;

	const FVector ListenerLocation = Owner->GetActorLocation();

	const TArray<FNoiseData> RecentNoises = SoundManagerRef->GetRecentNoises(RecentTimeWindow);

	if (RecentNoises.Num() == 0)
	{
		// Optional debug output
		if (bDrawDebug)
		{
			UE_LOG(LogTemp, Verbose, TEXT("No recent noises found within time window"));
		}
		return;
	}
	bool bHeardNoise = false;

	for (const FNoiseData& Noise : RecentNoises)
	{
		if (!Noise.Location.ContainsNaN() && Noise.Volume > 0.0f)
		{
			const float Distance = FVector::Dist(ListenerLocation, Noise.Location);
			const float EffectiveVolume = Noise.Volume * (1.0f / FMath::Max(MinimumVolumeThreshold, 0.01f));

			if (Distance <= HearingRadius && EffectiveVolume >= 1.0f)
			{
				bHeardNoise = true;
				OnNoiseHeard.Broadcast(Noise.Location, Noise.Volume);

				if (bDrawDebug)
				{
					DrawDebugSphere(GetWorld(), Noise.Location, 50.f, 12, FColor::Green, false, CheckInterval);
					DrawDebugLine(GetWorld(), ListenerLocation, Noise.Location, FColor::Yellow, false, CheckInterval);

					UE_LOG(LogTemp, Warning, TEXT("Heard noise at %s with volume %f"), *Noise.Location.ToString(),
					       Noise.Volume);
				}
			}
		}
	}
	if (bDrawDebug && !bHeardNoise)
	{
		UE_LOG(LogTemp, Verbose, TEXT("No valid noises heard within hearing radius"));
	}
}

void UNoiseListenerComponent::ValidateSoundManager()
{
	SoundManagerRef = Cast<ASoundManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASoundManager::StaticClass()));
	if (!SoundManagerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoundManager not found in level!"));
	}
}

void UNoiseListenerComponent::HandleNoiseHeard(const FVector& NoiseLocation, float Volume)
{
	// Validate owner
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleNoiseHeard: Component has no owner!"));
		return;
	}

	// 1. Primeiro verifique se o Owner é um APawn
	APawn* OwnerPawn = Cast<APawn>(Owner);

	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner não é um Pawn!"));
		return;
	}

	// 2. Agora sim pegue o Controller
	AMonsterAIController* AIController = Cast<AMonsterAIController>(OwnerPawn->GetController());

	if (AIController)
	{
		AIController->MoveToNoiseLocation(NoiseLocation);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Não encontrou AIController!"));
		return;
	}

	// Validate world
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleNoiseHeard: Cannot get world context!"));
		return;
	}

	// Validate noise location
	if (NoiseLocation.ContainsNaN())
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleNoiseHeard: Invalid noise location!"));
		return;
	}

	// Cast to character
	ACharacter* MonsterCharacter = Cast<ACharacter>(Owner);
	if (!MonsterCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleNoiseHeard: Owner is not a character!"));
		return;
	}

	UCharacterMovementComponent* MovementComp = MonsterCharacter->GetCharacterMovement();
	if (!MovementComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleNoiseHeard: No movement component found!"));
		return;
	}

	// Stop any existing movement
	// MovementComp->StopMovementImmediately();

	// Calculate direction to noise
	FVector ToNoise = NoiseLocation - Owner->GetActorLocation();
	float DistanceToNoise = ToNoise.Size();

	// Only move if noise is not at current location
	if (DistanceToNoise > KINDA_SMALL_NUMBER)
	{
		FVector Direction = ToNoise.GetSafeNormal();

		// Face the noise direction (yaw only)
		FRotator NewRotation = Direction.Rotation();
		Owner->SetActorRotation(FRotator(0, NewRotation.Yaw, 0));

		// Move toward noise
		MonsterCharacter->AddMovementInput(Direction, 1.0f);


		// Debug visualization
		if (bDrawDebug)
		{
			float DebugDuration = FMath::Max(CheckInterval, 1.0f); // Minimum 1 second visibility
			DrawDebugSphere(World, NoiseLocation, 30.f, 12, FColor::Red, false, DebugDuration);
			DrawDebugLine(World, Owner->GetActorLocation(), NoiseLocation, FColor::Green, false, DebugDuration);
			DrawDebugDirectionalArrow(World,
			                          Owner->GetActorLocation(),
			                          Owner->GetActorLocation() + Direction * 100.f,
			                          50.f, FColor::Blue, false, DebugDuration);
		}

		UE_LOG(LogTemp, Warning, TEXT("Moving toward noise at %s (Distance: %.2f)"),
		       *NoiseLocation.ToString(), DistanceToNoise);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Noise source is at current location - no movement needed"));
	}
}
