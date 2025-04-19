#include "MonsterAIController.h"
#include "MonsterCharacter.h"
#include "NotificationManager.h"
#include "HidingSpot.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "DrawDebugHelpers.h" // For debug drawing

#include "Navigation/PathFollowingComponent.h"

AMonsterAIController::AMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	ControlledMonster = nullptr;
	NotificationManager = nullptr;
	CurrentHidingSpot = nullptr;
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	SetTickableWhenPaused(true);
	ControlledMonster = Cast<AMonsterCharacter>(GetPawn());
	NotificationManager = Cast<ANotificationManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ANotificationManager::StaticClass()));
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ControlledMonster || !NotificationManager)
		return;

	// Use um nome diferente para a variável local ou use o membro da classe diretamente
	ANotificationManager* FoundNotificationManager = Cast<ANotificationManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ANotificationManager::StaticClass()));

	// Atribua ao membro da classe se necessário
	NotificationManager = FoundNotificationManager;


	// Verifica ruídos periodicamente
	if (NotificationManager)
	{
		TArray<FNotificationData> RecentNoises = NotificationManager->GetRecentNotifications(2.0f); // Ruídos dos últimos 2 segundos

		for (const FNotificationData& Noise : RecentNoises)
		{
			float Distance = FVector::Dist(Noise.Location, GetPawn()->GetActorLocation());
			if (CanHearNoise(Noise.Volume, Distance))
			{
				OnNoiseHeard(Noise.Location, Noise.Volume);
				break; // Reage apenas ao ruído mais relevante
			}
		}
	}

	// ProcessHeardNoises();
}

bool AMonsterAIController::CanHearNoise(float NoiseVolume, float Distance) const
{
	// Calcula a audibilidade baseada no volume e distância
	float EffectiveRadius = HearingRadius * NoiseSensitivity * NoiseVolume;
	return Distance <= EffectiveRadius;
}

void AMonsterAIController::OnNoiseHeard(FVector NoiseLocation, float NoiseVolume)
{
	LastHeardNoiseLocation = NoiseLocation;
	LastHeardNoiseVolume = NoiseVolume;

	if (AMonsterCharacter* Monster = Cast<AMonsterCharacter>(GetPawn()))
	{
		// Chama a função que dispara o evento e a lógica C++
		Monster->HearNoise(NoiseLocation, NoiseVolume);
	}
}

void AMonsterAIController::MoveToNoiseLocation(const FVector& NoiseLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("ENTROU EM MoveToNoiseLocation")); // FORÇA O DEBUG

	if (!GetPawn()) return;


	// Modern way to get navigation system
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSys)
	{
		UE_LOG(LogTemp, Error, TEXT("Navigation system not found!"));
	}

	FNavLocation NavLocation;
	const bool bProjectSuccess = NavSys->ProjectPointToNavigation(
		NoiseLocation,
		NavLocation,
		FVector(500.f, 500.f, 500.f) // Search extent
	);

	if (bProjectSuccess)
	{
		// Create move request
		FAIMoveRequest MoveRequest(NavLocation.Location);
		MoveRequest.SetAcceptanceRadius(100.f);
		MoveRequest.SetUsePathfinding(true);

		// Execute move
		FPathFollowingRequestResult Result = MoveTo(MoveRequest);

		// Debug
		if (bDrawDebug)
		{
			const float DebugDuration = 5.f;
			DrawDebugSphere(GetWorld(), NavLocation.Location, 50.f, 12, FColor::Green, false, DebugDuration);
			DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), NavLocation.Location, FColor::Yellow, false,
			              DebugDuration);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to project noise location to navmesh!"));
	}
}


void AMonsterAIController::ProcessHeardNoises()
{
	const TArray<FNotificationData> RecentNoises = NotificationManager->GetRecentNotifications(MaxNoiseAge);

	for (const FNotificationData& Noise : RecentNoises)
	{
		float Distance = FVector::Dist(Noise.Location, ControlledMonster->GetActorLocation());

		if (Distance <= NoiseDetectionRadius && Noise.Volume >= ControlledMonster->GetNoiseSensitivity())
		{
			// Decide entre investigar, perseguir ou fugir
			if (ControlledMonster->ShouldFlee())
			{
				FleeAndHide();
			}
			else
			{
				MoveToLocation(Noise.Location);
			}

			break; // Apenas o barulho mais relevante
		}
	}
}

void AMonsterAIController::FleeAndHide()
{
	FVector PlayerLocation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	FVector EscapeDirection = ControlledMonster->GetActorLocation() - PlayerLocation;
	EscapeDirection.Normalize();

	FVector EscapeTarget = ControlledMonster->GetActorLocation() + EscapeDirection * MinEscapeDistance;

	AHidingSpot* Spot = FindBestHidingSpot(ControlledMonster->GetCreatureSize());
	if (Spot)
	{
		CurrentHidingSpot = Spot;
		Spot->Occupy();
		MoveToLocation(Spot->HidingPosition);
	}
	else
	{
		MoveToLocation(EscapeTarget); // Vai pra região mais escura
	}
}

AHidingSpot* AMonsterAIController::FindBestHidingSpot(ECreatureSize MonsterSize) const
{
	TArray<AActor*> FoundSpots;
	UGameplayStatics::GetAllActorsOfClass(GEngine->GetWorld(), AHidingSpot::StaticClass(), FoundSpots);

	AHidingSpot* BestSpot = nullptr;
	float BestScore = TNumericLimits<float>::Max();

	for (AActor* Actor : FoundSpots)
	{
		AHidingSpot* Spot = Cast<AHidingSpot>(Actor);
		if (Spot && Spot->CanBeUsedBy(MonsterSize))
		{
			float Distance = FVector::Dist(ControlledMonster->GetActorLocation(), Spot->HidingPosition);

			// Simples critério de proximidade (pode evoluir com pesos)
			if (Distance < BestScore)
			{
				BestScore = Distance;
				BestSpot = Spot;
			}
		}
	}

	return BestSpot;
}


void AMonsterAIController::StopNoiseInvestigation()
{
	StopMovement();
	CurrentNoiseLocation = FVector::ZeroVector;

	// Implemente aqui qualquer lógica adicional quando a investigação terminar
	UE_LOG(LogTemp, Warning, TEXT("Investigação do ruído concluída"));
}

void AMonsterAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	const FString ResultString = UEnum::GetValueAsString(Result.Code);
	UE_LOG(LogTemp, Verbose, TEXT("Movimento concluído com resultado: %s"), *ResultString);


	switch (Result.Code)
	{
	case EPathFollowingResult::Success:
		UE_LOG(LogTemp, Log, TEXT("Chegou exatamente ao local do ruído!"));
		break;

	case EPathFollowingResult::Blocked:
		UE_LOG(LogTemp, Warning, TEXT("Caminho bloqueado! Tentando contornar..."));
		break;

	case EPathFollowingResult::OffPath:
		UE_LOG(LogTemp, Warning, TEXT("Saiu do caminho, mas está próximo do destino"));
		break;

	case EPathFollowingResult::Aborted:
		UE_LOG(LogTemp, Warning, TEXT("Movimento abortado (novo comando recebido)"));
		break;

	case EPathFollowingResult::Invalid:
		UE_LOG(LogTemp, Error, TEXT("Requisição inválida!"));
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Resultado inesperado: %s"), *ResultString);
		break;
	}
}
