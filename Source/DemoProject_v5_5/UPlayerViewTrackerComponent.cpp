#include "UPlayerViewTrackerComponent.h"

#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

UPlayerViewTrackerComponent::UPlayerViewTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerViewTrackerComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateViewData();
}

void UPlayerViewTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateViewData();
}

void UPlayerViewTrackerComponent::UpdateViewData()
{
	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	// Usa a câmera principal do player
	ACharacter* Character = Cast<ACharacter>(Owner);
	if (Character)
	{
		UCameraComponent* Camera = Character->FindComponentByClass<UCameraComponent>();
		if (Camera)
		{
			LastViewLocation = Camera->GetComponentLocation();
			LastViewDirection = Camera->GetForwardVector();
		}
	}
}

bool UPlayerViewTrackerComponent::IsLocationInView(const FVector& Location, float& OutDotProduct) const
{
	FVector ToTarget = Location - LastViewLocation;
	float Distance = ToTarget.Size();
	if (Distance > MaxViewDistance)
		return false;

	ToTarget.Normalize();
	OutDotProduct = FVector::DotProduct(ToTarget, LastViewDirection);

	float Threshold = FMath::Cos(FMath::DegreesToRadians(FieldOfViewAngleDegrees / 2));
	return OutDotProduct >= Threshold;
}

bool UPlayerViewTrackerComponent::IsActorInView(AActor* TargetActor, float& OutDotProduct) const
{
	if (!TargetActor) return false;
	return IsLocationInView(TargetActor->GetActorLocation(), OutDotProduct);
}
