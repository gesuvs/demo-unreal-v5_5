// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterCharacter.generated.h"

// Declaração do delegate para eventos de ruído
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMonsterNoiseSignature, FVector, NoiseLocation, float, NoiseVolume);

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    Idle,
    Investigating,
    Hunting,
    Fleeing,
    Hiding,
    Stalking
};

UENUM(BlueprintType)
enum class ECreatureSize : uint8
{
    Small,
    Medium,
    Large
};

UCLASS()
class DEMOPROJECT_V5_5_API AMonsterCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMonsterCharacter();

    // ====== DELEGATES E EVENTOS ======
    UPROPERTY(BlueprintAssignable, Category = "AI|Events")
    FMonsterNoiseSignature OnNoiseDetected;

    UFUNCTION(BlueprintImplementableEvent, Category = "AI")
    void OnNoiseHeardEvent(FVector NoiseLocation, float NoiseVolume);

    // ====== FUNÇÕES PÚBLICAS ======
    UFUNCTION(BlueprintCallable, Category = "AI|Events")
    void HearNoise(FVector NoiseLocation, float NoiseVolume);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void OnNoiseHeard(const FVector& NoiseLocation, float Volume);

    UFUNCTION(BlueprintCallable, Category = "AI")
    float GetNoiseSensitivity() const;

    UFUNCTION(BlueprintCallable, Category = "AI")
    bool ShouldFlee() const;

    UFUNCTION(BlueprintCallable, Category = "AI")
    ECreatureSize GetCreatureSize() const { return CreatureSize; }

    // ====== PROPRIEDADES EDITÁVEIS ======
    /** Configurações de som e movimento */
    UPROPERTY(EditAnywhere, Category = "AI|Sound")
    USoundBase* NoiseReactionSound;

    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float InvestigationSpeed = 300.0f;

    /** Estado e velocidade */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EMonsterState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float PatrolSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float HuntSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float FleeSpeed;

    /** Componentes */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class UNoiseListenerComponent* NoiseListener;

    /** Tamanho e características */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    float MonsterSize;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    float MinFleeDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Hearing")
    float NoiseSensitivity = 100.0f;

    /** Propriedades básicas */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float Speed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float HearingRange;

protected:
    // ====== FUNÇÕES PROTEGIDAS ======
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    void HandleIdleState(float DeltaTime);
    void HandleInvestigatingState(float DeltaTime);
    void HandleHuntingState(float DeltaTime);
    void HandleFleeingState(float DeltaTime);
    void HandleHidingState(float DeltaTime);
    void HandleStalkingState(float DeltaTime);
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION()
    virtual void HandleNoiseResponse(FVector NoiseLocation, float NoiseVolume);

    /** Tamanho do monstro (usado para validar esconderijos) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Hiding")
    ECreatureSize CreatureSize = ECreatureSize::Medium;

private:
    // ====== FUNÇÕES PRIVADAS ======
    void AttemptToHide();
    void SetState(EMonsterState NewState);
    EMonsterState GetState() const;
    void MoveToLocation(FVector Location);
    void AttackPlayer();
};