// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseEmitterComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEMOPROJECT_V5_5_API UNoiseEmitterComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UNoiseEmitterComponent();

protected:
    // Called when the game starts
    void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

public:
    // Função que é chamada para registrar o ruído
    UFUNCTION(BlueprintCallable, Category = "Sound")
    void EmitNoise(float Volume, FVector Location);

    // Evento que será chamado na colisão de um objeto (ou quando um objeto for jogado)
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
               FVector NormalImpulse, const FHitResult& Hit);


    // Armazenar o volume de barulho gerado
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    float BaseNoiseVolume = 100.0f;

    // A referência para o sistema de SoundManager que vai processar o ruído
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    class ASoundManager* SoundManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NoiseEmitter")
    UNoiseEmitterComponent* NoiseEmitterComponent;

    // Função chamada quando o botão esquerdo é pressionado
    void OnLeftMousePressed();
    APawn* GetControlledPawn(AActor* OwnerActor) const;
    void EmitNoiseFromActor(AActor* SourceActor);

    UFUNCTION(BlueprintCallable, Category = "Input")
    void BindInputs(APlayerController* PlayerController);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void UnbindInputs();

    void EndPlay(EEndPlayReason::Type EndPlayReason);

    UPROPERTY(EditAnywhere, Category = "Noise")
    bool bUseOwnerLocationAsFallback = true; // Inicialização direta

};
