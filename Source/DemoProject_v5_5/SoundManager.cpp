#include "SoundManager.h"
#include "Engine/World.h"

static const float DEFAULT_NOISE_MEMORY_DURATION = 10.0f; // ruídos duram 10s
static const float DEFAULT_CLEANUP_INTERVAL = 1.0f; // limpa a cada 1s

ASoundManager::ASoundManager()
{
    PrimaryActorTick.bCanEverTick = true;

    NoiseMemoryDuration = DEFAULT_NOISE_MEMORY_DURATION;
    NoiseCleanupInterval = DEFAULT_CLEANUP_INTERVAL;
    CleanupTimer = 0.0f;
}

void ASoundManager::BeginPlay()
{
    Super::BeginPlay();
}

void ASoundManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CleanupTimer += DeltaTime;
    if (CleanupTimer >= NoiseCleanupInterval)
    {
        CleanupOldNoises();
        CleanupTimer = 0.0f;
    }
}

void ASoundManager::RegisterNoise(FVector Location, float Volume)
{
    const float CurrentTime = GetWorld()->GetTimeSeconds();

    if (NoiseCount >= MaxNoiseEvents)
    {
        NoiseQueue[NoiseStartIndex] = FNoiseData{Location, Volume, CurrentTime};
        NoiseStartIndex = (NoiseStartIndex + 1) % MaxNoiseEvents; // Move o inicio
    }
    else
    {
        NoiseQueue.Add(FNoiseData{Location, Volume, CurrentTime});
        ++NoiseCount;
    }

    // Debug visual
    float Radius = Volume * 100.f; // Ajuste o multiplicador conforme necessário
    DrawDebugSphere(GetWorld(), Location, Radius, 12, FColor::Red, false, 2.f);


#if WITH_EDITOR
    UE_LOG(LogTemp, Warning, TEXT("[SoundManager] Ruído registrado: Volume = %.2f | Local = %s"), Volume,
           *Location.ToString());
#endif
}

TArray<FNoiseData> ASoundManager::GetRecentNoises(float WithinSeconds) const
{
    TArray<FNoiseData> RecentNoises;

    if (!GetWorld() || NoiseCount <= 0 || MaxNoiseEvents <= 0)
    {
        return RecentNoises; // Return empty array if invalid state
    }


    const float CurrentTime = GetWorld()->GetTimeSeconds();
    const float MinValidTime = CurrentTime - WithinSeconds;

    // Reserve space for worst-case scenario to avoid multiple allocations
    RecentNoises.Reserve(FMath::Min(NoiseCount, MaxNoiseEvents));

    // Percorre apenas os ruídos válidos
    for (int32 i = 0; i < NoiseCount; ++i)
    {
        const int32 Index = (NoiseStartIndex + i) % MaxNoiseEvents;

        if (Index >= 0 && Index < NoiseQueue.Num() && NoiseQueue[Index].Timestamp >= MinValidTime)
        {
            RecentNoises.Add(NoiseQueue[Index]);
        }

        // if (CurrentTime - NoiseQueue[Index].Timestamp <= WithinSeconds)
        // {
        //     RecentNoises.Add(NoiseQueue[Index]);
        // }
    }

    return RecentNoises;
}

// Remove ruídos que já expiraram do array
void ASoundManager::CleanupOldNoises()
{
    const float CurrentTime = GetWorld()->GetTimeSeconds();
    const float ExpireTime = CurrentTime - NoiseMemoryDuration;

    // Remove os ruídos que são mais antigos do que a memória permitida
    NoiseQueue.RemoveAll([ExpireTime](const FNoiseData& Noise)
    {
        return Noise.Timestamp < ExpireTime;
    });
}
