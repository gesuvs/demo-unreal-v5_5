// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterCharacter.h"
#include "MonsterAIController.h"
#include "NoiseListenerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AMonsterCharacter::AMonsterCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Inicialização dos componentes
    NoiseListener = CreateDefaultSubobject<UNoiseListenerComponent>(TEXT("NoiseListener"));

    // Configurações de movimento
    PatrolSpeed = 200.0f;
    HuntSpeed = 500.0f;
    FleeSpeed = 300.0f;
    GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

    // Atributos do monstro
    MonsterSize = 1.0f;
    MinFleeDistance = 1000.0f;
    Health = 100.0f;
    Speed = 300.0f;
    HearingRange = 1000.0f;
    NoiseSensitivity = 100.0f;

    // Estado inicial
    CurrentState = EMonsterState::Idle;
}

void AMonsterCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Configura o listener de ruídos
    if (NoiseListener)
    {
        NoiseListener->OnNoiseHeard.AddDynamic(this, &AMonsterCharacter::OnNoiseHeard);
    }
}

void AMonsterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Comportamento baseado no estado
    switch (CurrentState)
    {
    case EMonsterState::Idle:
        HandleIdleState(DeltaTime);
        break;
    case EMonsterState::Investigating:
        HandleInvestigatingState(DeltaTime);
        break;
    case EMonsterState::Hunting:
        HandleHuntingState(DeltaTime);
        break;
    case EMonsterState::Fleeing:
        HandleFleeingState(DeltaTime);
        break;
    case EMonsterState::Hiding:
        HandleHidingState(DeltaTime);
        break;
    case EMonsterState::Stalking:
        HandleStalkingState(DeltaTime);
        break;
    }
}

// ====== IMPLEMENTAÇÕES DE ESTADO ======

void AMonsterCharacter::HandleIdleState(float DeltaTime)
{
    // Comportamento padrão de espera
}

void AMonsterCharacter::HandleInvestigatingState(float DeltaTime)
{
    // Lógica de investigação de ruídos
}

void AMonsterCharacter::HandleHuntingState(float DeltaTime)
{
    // Comportamento agressivo de caça
}

void AMonsterCharacter::HandleFleeingState(float DeltaTime)
{
    // Comportamento de fuga
}

void AMonsterCharacter::HandleHidingState(float DeltaTime)
{
    // Comportamento enquanto escondido
}

void AMonsterCharacter::HandleStalkingState(float DeltaTime)
{
    // Comportamento furtivo
}

// ====== SISTEMA DE RUÍDOS ======

void AMonsterCharacter::OnNoiseHeard(const FVector& NoiseLocation, float Volume)
{
    float EffectiveVolume = Volume * (1.0f / NoiseSensitivity);
    HearNoise(NoiseLocation, EffectiveVolume);
}

void AMonsterCharacter::HearNoise(FVector NoiseLocation, float NoiseVolume)
{
    // Dispara o evento multicast
    OnNoiseDetected.Broadcast(NoiseLocation, NoiseVolume);

    // Implementação em C++
    HandleNoiseResponse(NoiseLocation, NoiseVolume);

    // Evento Blueprint implementável
    OnNoiseHeardEvent(NoiseLocation, NoiseVolume);
}

void AMonsterCharacter::HandleNoiseResponse(FVector NoiseLocation, float NoiseVolume)
{
    // Debug visual
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
                                         FString::Printf(TEXT("Heard noise at: %s (Vol: %.2f)"),
                                                         *NoiseLocation.ToString(), NoiseVolume));
    }

    // 1. Orientação para o ruído
    FVector Direction = NoiseLocation - GetActorLocation();
    Direction.Z = 0;
    if (!Direction.IsNearlyZero())
    {
        FRotator NewRotation = Direction.Rotation();
        SetActorRotation(NewRotation);
    }

    // 2. Som de reação
    if (NoiseReactionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, NoiseReactionSound, GetActorLocation());
    }

    // 3. Lógica de estado
    if (NoiseVolume > 0.7f) // Ruído alto
    {
        SetState(EMonsterState::Hunting);
        if (AMonsterAIController* AIController = Cast<AMonsterAIController>(GetController()))
        {
            AIController->MoveToLocation(NoiseLocation);
        }
    }
    else // Ruído baixo
    {
        SetState(EMonsterState::Investigating);
        if (AMonsterAIController* AIController = Cast<AMonsterAIController>(GetController()))
        {
            AIController->MoveToLocation(NoiseLocation, -1.0f, true, true);
        }
    }
}

// ====== SISTEMA DE ESTADOS ======

void AMonsterCharacter::SetState(EMonsterState NewState)
{
    if (CurrentState == NewState) return;

    EMonsterState PreviousState = CurrentState;
    CurrentState = NewState;

    // Ajusta velocidade baseada no estado
    switch (CurrentState)
    {
    case EMonsterState::Idle:
        GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
        break;
    case EMonsterState::Investigating:
        GetCharacterMovement()->MaxWalkSpeed = InvestigationSpeed;
        break;
    case EMonsterState::Hunting:
        GetCharacterMovement()->MaxWalkSpeed = HuntSpeed;
        break;
    case EMonsterState::Fleeing:
        GetCharacterMovement()->MaxWalkSpeed = FleeSpeed;
        break;
    default:
        break;
    }

    // Debug
    UE_LOG(LogTemp, Warning, TEXT("State changed from %d to %d"), (int)PreviousState, (int)CurrentState);
}

EMonsterState AMonsterCharacter::GetState() const
{
    return CurrentState;
}

// ====== COMPORTAMENTOS ESPECÍFICOS ======

void AMonsterCharacter::AttemptToHide()
{
    SetState(EMonsterState::Hiding);

    // Lógica para encontrar esconderijo mais próximo
    // (Implementação detalhada iria aqui)
}

void AMonsterCharacter::AttackPlayer()
{
    // Lógica básica de ataque
    AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (Player)
    {
        SetState(EMonsterState::Hunting);
        if (AMonsterAIController* AIController = Cast<AMonsterAIController>(GetController()))
        {
            AIController->MoveToActor(Player);
        }
    }
}

// ====== FUNÇÕES AUXILIARES ======

float AMonsterCharacter::GetNoiseSensitivity() const
{
    return NoiseSensitivity;
}

bool AMonsterCharacter::ShouldFlee() const
{
    // Foge se a saúde estiver baixa ou por outras condições
    return (Health < 50.0f) || (CurrentState == EMonsterState::Fleeing);
}

void AMonsterCharacter::MoveToLocation(FVector Location)
{
    if (AMonsterAIController* AIController = Cast<AMonsterAIController>(GetController()))
    {
        AIController->MoveToLocation(Location);
    }
}

void AMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
