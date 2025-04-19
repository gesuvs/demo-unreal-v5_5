#include "NoiseEmitterComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"

UNoiseEmitterComponent::UNoiseEmitterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Definindo constantes para evitar números mágicos
const float MAX_NOISE_VOLUME = 1000.0f; // Volume máximo de ruído
const float MIN_NOISE_VOLUME = 0.0f; // Volume mínimo (não pode ser negativo)
const float IMPACT_VELOCITY_SCALE = 1.0f; // Fator de escala para o impacto (ajustável, se necessário)


void UNoiseEmitterComponent::BeginPlay()
{
	Super::BeginPlay();

	// Inicializa SoundManager
	TArray<AActor*> FoundManagers;


	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASoundManager::StaticClass(), FoundManagers);

	if (FoundManagers.Num() > 0)
	{
		SoundManager = Cast<ASoundManager>(FoundManagers[0]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SoundManager não encontrado no mundo!"));
	}


	// Se o componente tiver um objeto de som gerenciador, vamos ouvir colisões
	// Configura colisão

	if (AActor* Owner = GetOwner())
	{
		UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(
			Owner->GetComponentByClass(UPrimitiveComponent::StaticClass()));

		if (PrimitiveComp)
		{
			PrimitiveComp->OnComponentHit.AddDynamic(this, &UNoiseEmitterComponent::OnHit);
		}
	}

	// Configura input
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		BindInputs(PC);
	}
}

void UNoiseEmitterComponent::EmitNoise(float Volume, FVector Location)
{
	UE_LOG(LogTemp, Warning, TEXT("Emitindo ruído: Volume=%.2f, Local=(%.1f, %.1f, %.1f)"), Volume, Location.X,
	       Location.Y, Location.Z);

	FVector NoiseLocation = FVector::ZeroVector;
	FHitResult HitResult;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);
		NoiseLocation = HitResult.Location;
	}

	// 2. Emitir o ruído na posição do mouse
	if (NoiseLocation != FVector::ZeroVector && SoundManager && SoundManager->IsValidLowLevel())
	{
		float ClampedVolume = FMath::Clamp(Volume, MIN_NOISE_VOLUME, MAX_NOISE_VOLUME);

		SoundManager->RegisterNoise(NoiseLocation, ClampedVolume);
		// Debug
		DrawDebugSphere(GetWorld(), NoiseLocation, 50.f, 12, FColor::Green, false, 2.f);


		// Debug visual
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
			                                 FString::Printf(TEXT("Ruído emitido em: %s (Volume: %.2f)"),
			                                                 *NoiseLocation.ToString(), Volume));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Tentativa de emitir ruído sem SoundManager válido!"));
	}
}

void UNoiseEmitterComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Cálculo de volume com base na velocidade de impacto (quanto maior a velocidade, mais barulho)
	float ImpactVelocity = Hit.ImpactNormal.Size();

	// Calcule o volume de ruído gerado com base na velocidade de impacto
	float NoiseVolume = FMath::Clamp(ImpactVelocity * IMPACT_VELOCITY_SCALE * BaseNoiseVolume, MIN_NOISE_VOLUME,
	                                 MAX_NOISE_VOLUME);

	// Emite o ruído no local da colisão
	EmitNoise(NoiseVolume, Hit.ImpactPoint);
}

void UNoiseEmitterComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &UNoiseEmitterComponent::OnLeftMousePressed);
	}
}

void UNoiseEmitterComponent::OnLeftMousePressed()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner não encontrado"));
		return;
	}

	// 1. Tentar obter o PlayerController diretamente
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController não encontrado diretamente"));
		// 2. Tentar via Pawn
		APawn* OwnerPawn = Cast<APawn>(Owner);
		if (!OwnerPawn)
		{
			OwnerPawn = GetControlledPawn(Owner);
		}

		if (OwnerPawn)
		{
			PC = OwnerPawn->GetController<APlayerController>();
		}
	}

	// 3. Obter posição do mouse
	if (PC)
	{
		FHitResult HitResult;
		bool bHitSuccessful = PC->GetHitResultUnderCursorByChannel(
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			true,
			HitResult);

		if (bHitSuccessful)
		{
			UE_LOG(LogTemp, Log, TEXT("Ruído emitido em: %s"), *HitResult.Location.ToString());
			EmitNoise(BaseNoiseVolume, HitResult.Location);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController não encontrado de nenhuma forma"));
	}


	// AActor* Owner = GetOwner();
	// if (!Owner)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Owner nao encontrado"));
	// 	return;
	// }
	//
	// // Obter a posição do mouse no mundo
	// APawn* OwnerPawn = Cast<APawn>(Owner);
	// if (!OwnerPawn) OwnerPawn = GetControlledPawn(Owner);
	//
	// if (OwnerPawn)
	// {
	// 	APlayerController* PC = OwnerPawn->GetController<APlayerController>();
	// 	if (PC)
	// 	{
	// 		FHitResult HitResult;
	// 		if (PC->GetHitResultUnderCursor(ECC_Visibility, true, HitResult))
	// 		{
	// 			// Emitir ruído na posição do clique
	// 			EmitNoise(BaseNoiseVolume, HitResult.Location);
	// 		}
	// 	}
	// }
	//
	// // Fallback
	// if (bUseOwnerLocationAsFallback)
	// {
	// 	EmitNoiseFromActor(Owner);
	// }
}

APawn* UNoiseEmitterComponent::GetControlledPawn(AActor* OwnerActor) const
{
	if (AController* Controller = Cast<AController>(OwnerActor))
	{
		return Controller->GetPawn();
	}
	return nullptr;
}

void UNoiseEmitterComponent::EmitNoiseFromActor(AActor* SourceActor)
{
	if (SourceActor && SoundManager)
	{
		FVector NoiseLocation = SourceActor->GetActorLocation();
		SoundManager->RegisterNoise(NoiseLocation, BaseNoiseVolume);
		DrawDebugSphere(GetWorld(), NoiseLocation, 50.f, 12, FColor::Red, false, 2.f);
	}
}

void UNoiseEmitterComponent::BindInputs(APlayerController* PlayerController)
{
	if (PlayerController && PlayerController->InputComponent)
	{
		PlayerController->InputComponent->BindAction(
			"Fire",
			IE_Pressed,
			this,
			&UNoiseEmitterComponent::OnLeftMousePressed
		).bConsumeInput = true;
	}
}

void UNoiseEmitterComponent::UnbindInputs()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->InputComponent)
		{
			PC->InputComponent->RemoveActionBinding("Fire", IE_Pressed);
		}
	}
}

void UNoiseEmitterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindInputs();
	Super::EndPlay(EndPlayReason);
}
