#include "MonsterAIController.h"
#include "MonsterCharacter.h"
#include "DrawDebugHelpers.h" // For debug drawing

#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"

AMonsterAIController::AMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Cria o componente de percepção
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionComponent); // importante para GetPerceptionComponent funcionar

	ControlledMonster = nullptr;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	if (HearingConfig)
	{
		HearingConfig->HearingRange = 2000.0f;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

		PerceptionComponent->ConfigureSense(*HearingConfig);
		PerceptionComponent->SetDominantSense(HearingConfig->GetSenseImplementation());
	}
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	ControlledMonster = Cast<AMonsterCharacter>(GetPawn());

	if (GetPerceptionComponent())
	{
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(
			this, &AMonsterAIController::OnTargetPerceptionUpdated);
		UE_LOG(LogTemp, Warning, TEXT("PerceptionComponent configurado com sucesso"));
	}
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UAISense_Hearing* HearingSense = NewObject<UAISense_Hearing>();

	const FAISenseID HearingSenseID = HearingSense->GetSenseID();

	if (Stimulus.WasSuccessfullySensed() && Stimulus.Type == HearingSenseID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inimigo ouviu som em %s"), *Stimulus.StimulusLocation.ToString());

		if (ControlledMonster)
		{
			FVector MonsterLocation = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
			float DistanceToNoise = FVector::Dist(MonsterLocation, Stimulus.StimulusLocation);

			// Mesmo valor usado no UAISenseConfig_Hearing
			float MaxHearingRange = HearingConfig ? HearingConfig->HearingRange : 2000.f;

			UE_LOG(LogTemp, Warning, TEXT("Distância até o barulho: %f"), DistanceToNoise);

			if (DistanceToNoise <= MaxHearingRange)
			{
				// Está dentro do alcance, pode mover até o som
				MoveToLocation(Stimulus.StimulusLocation);

				UE_LOG(LogTemp, Warning, TEXT("Indo investigar barulho..."));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Som fora do alcance, ignorando"));
			}
		}
	}
}


void AMonsterAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	const FString ResultString = UEnum::GetValueAsString(Result.Code);
	UE_LOG(LogTemp, Warning, TEXT("Movimento concluído com resultado: %s"), *ResultString);

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

float AMonsterAIController::GetHearingRange() const
{
	return HearingConfig ? HearingConfig->HearingRange : 0.0f;
}
