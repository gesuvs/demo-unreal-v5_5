// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterCustom.h"

#include "Perception/AISense_Hearing.h"

// Sets default values
APlayerCharacterCustom::APlayerCharacterCustom()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	if (HearingConfig)
		HearingConfig->HearingRange = 2000.0f;
}

// Called when the game starts or when spawned
void APlayerCharacterCustom::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharacterCustom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacterCustom::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Teste", IE_Pressed, this, &APlayerCharacterCustom::TestMakeNoise);
}


void APlayerCharacterCustom::TestMakeNoise()
{
	FVector SoundLocation = GetActorLocation(); // Posição do personagem

	float Volume = HearingConfig->HearingRange; // Igual ao alcance


	// Log para verificar se o som foi gerado corretamente
	UE_LOG(LogTemp, Warning, TEXT("Barulho gerado em %s"), *SoundLocation.ToString());


	// Envia o estímulo para o sistema de percepção do inimigo
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		SoundLocation, // Localização do som
		Volume, // Intensidade do som
		this, // O ator que gerou o som
		Volume // A distância máxima do som
	);

	DrawDebugSphere(GetWorld(), SoundLocation, Volume, 12, FColor::Red, false, 2.0f);
}
