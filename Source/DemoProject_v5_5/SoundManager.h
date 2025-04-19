#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundManager.generated.h"

// Estrutura que representa um dado de som registrado no mundo
USTRUCT(BlueprintType)
struct FNoiseData
{
    GENERATED_BODY() // Adicione isso para estruturas USTRUCT
    
    FVector Location;
    float Volume;
    float Timestamp;

    FNoiseData() : Location(FVector::ZeroVector), Volume(0.0f), Timestamp(0.0f)
    {
    }

    FNoiseData(FVector InLocation, float InVolume, float InTimestamp)
        : Location(InLocation), Volume(InVolume), Timestamp(InTimestamp)
    {
    }
};

UCLASS()
class DEMOPROJECT_V5_5_API ASoundManager : public AActor
{
    GENERATED_BODY()

public:
    ASoundManager();

    // Registra um novo ruído no mundo
    void RegisterNoise(FVector Location, float Volume);

    // Retorna os ruídos recentes dentro de um intervalo de tempo (em segundos)
    TArray<FNoiseData> GetRecentNoises(float WithinSeconds) const;

protected:
    void Tick(float DeltaTime) override;
    void BeginPlay() override;

private:
    // Armazena os eventos de ruído ocorridos
    TArray<FNoiseData> NoiseQueue;

    int32 NoiseStartIndex = 0; // Índice do primeiro ruído válido
    int32 NoiseCount = 0; // Número atual de ruídos válidos


    // Constantes para otimizar performance
    static constexpr int32 MaxNoiseEvents = 256; // Número máximo de ruídos armazenados
    static constexpr float DefaultNoiseMemoryWindow = 5.0f; // Tempo que um ruído é válido

    // Intervalo de tempo que um ruído pode permanecer na fila (em segundos)
    UPROPERTY(EditAnywhere, Category = "Noise")
    float NoiseMemoryDuration;

    // Frequência com que o sistema limpa ruídos antigos (em segundos)
    UPROPERTY(EditAnywhere, Category = "Performance")
    float NoiseCleanupInterval;

    // Tempo acumulado para controle da limpeza de ruídos
    float CleanupTimer;


    void CleanupOldNoises();
};
