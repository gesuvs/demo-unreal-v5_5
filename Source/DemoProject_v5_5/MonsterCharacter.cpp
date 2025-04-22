#include "MonsterCharacter.h"
#include "DrawDebugHelpers.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Health = 100.0f;
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrimaryActorTick.bCanEverTick = true;
}
