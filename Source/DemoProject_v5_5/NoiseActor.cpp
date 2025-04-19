// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseActor.h"
#include "Components/SceneComponent.h"  // Importante para usar SceneComponent
#include "NoiseEmitterComponent.h"

// Sets default values for this component's properties
ANoiseActor::ANoiseActor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	// Criar um componente raiz do tipo SceneComponent
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));; // Definir como o RootComponent
	NoiseEmitter = CreateDefaultSubobject<UNoiseEmitterComponent>(TEXT("NoiseEmitter"));

	// Garante que ele será tratado como parte do ator
	AddOwnedComponent(NoiseEmitter);
	SetRootComponent(Root);

	// Criar o NoiseEmitterComponent
	NoiseEmitter->SetupAttachment(Root);
}


// Called when the game starts
void ANoiseActor::BeginPlay()
{
	Super::BeginPlay();

	// Verifique se o componente foi criado corretamente
	if (NoiseEmitter)
	{
		UE_LOG(LogTemp, Warning, TEXT("NoiseEmitterComponent foi criado com sucesso!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NoiseEmitterComponent NÃO foi criado!"));
	}
}


// Called every frame
void ANoiseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
