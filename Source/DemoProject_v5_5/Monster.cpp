#include "Monster.h"
#include "HidingSpot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "AIController.h"

// Construtor
AMonster::AMonster()
{
	// Inicialização dos valores padrões
	MaxSoundRange = 1000.0f;
	SoundSensitivity = 1.0f;
	bIsAggressive = true;
	HideChance = 0.5f;
	bCanFlee = true;
	bIsHiding = false;
	MinFleeDistance = 2000.0f; // Distância mínima de fuga
}

void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Comportamento do monstro: se estiver escondido, não faz nada
	if (bIsHiding)
	{
		// O monstro pode ficar aguardando até que o jogador se afaste para realizar outro comportamento
		return;
	}

	// Comportamento normal do monstro (verificar se está perto de um esconderijo, se deve atacar ou fugir)
	if (bIsAggressive)
	{
		AttackPlayer();
	}
	else if (bCanFlee)
	{
		Flee();
	}
}

void AMonster::HearSound(FVector SoundLocation, float SoundVolume)
{
	// Verificar se o monstro ouviu o som
	float Distance = FVector::Dist(GetActorLocation(), SoundLocation);
	if (Distance <= MaxSoundRange * SoundSensitivity)
	{
		// O som foi ouvido, agora o monstro deve tomar uma ação
		if (FMath::RandRange(0.0f, 1.0f) < HideChance)
		{
			Hide(); // O monstro tenta se esconder
		}
		else
		{
			AttackPlayer(); // O monstro ataca
		}
	}
}

void AMonster::Hide()
{
	// Lógica de esconderijo (simplesmente mover para o ponto de esconderijo, por exemplo)
	bIsHiding = true;
	SetActorLocation(HidingSpot);
}

void AMonster::AttackPlayer()
{
	// Lógica de ataque (simplesmente atacar o jogador)
	AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (Player)
	{
		// Código de ataque (por exemplo, fazer o monstro se aproximar do jogador)
		FVector PlayerLocation = Player->GetActorLocation();

		// Verificando se o controlador é do tipo AAIController
		AAIController* AIController = Cast<AAIController>(GetController());

		if (AIController)
		{
			AIController->MoveToLocation(PlayerLocation);
		}
	}
}

void AMonster::Flee()
{
	// Buscar o jogador
	AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (!Player)
	{
		return;
	}

	// Calcular a direção oposta ao jogador
	FVector Direction = GetActorLocation() - Player->GetActorLocation();
	Direction.Normalize();

	// Gerar uma posição de fuga aleatória dentro de um raio
	FVector FleeDestination = GetActorLocation() + Direction * MinFleeDistance;

	// Verificar se o monstro encontrou um esconderijo durante a fuga
	AHidingSpot* AvailableSpot = FindAvailableHidingSpot();
	if (AvailableSpot)
	{
		// Caso o esconderijo tenha capacidade, o monstro vai para lá
		SetActorLocation(GetActorLocation() + FleeDestination);
		AvailableSpot->SpotData.AddMonster(1); // Monstro se escondeu no esconderijo
	}
	else
	{
		// Caso não haja esconderijo disponível, o monstro apenas foge
		SetActorLocation(FleeDestination);
	}


	// if (!bIsHiding)
	//    {
	//        bIsHiding = true;
	//        SetActorLocation(HidingSpot); // Fugindo para o esconderijo
	//    } // Lógica de fuga (simplesmente fugir de volta para o ponto de esconderijo ou outra direção)
}

AHidingSpot* AMonster::FindAvailableHidingSpot()
{
	// Itera pelos esconderijos e tenta encontrar um que tenha capacidade
	for (AHidingSpot* Spot : HidingSpots)
	{
		if (Spot && Spot->IsAvailable(1))
		{
			return Spot; // Retorna o primeiro esconderijo disponível
		}
	}

	return nullptr; // Nenhum esconderijo disponível
}
