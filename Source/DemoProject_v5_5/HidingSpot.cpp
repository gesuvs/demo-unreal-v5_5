#include "HidingSpot.h"

AHidingSpot::AHidingSpot()
{
	PrimaryActorTick.bCanEverTick = true;

	SpotType = EHidingSpotType::UnderBed;
	MaxCreatureSize = 100.0f; // Valor padrão
	HidingPosition = GetActorLocation(); // Default para a posição do ator
	bIsOccupied = false;

	SpotData.CurrentOccupancy = 0.0f;
}

bool AHidingSpot::IsAvailable(int32 NumMonsters) const
{
	return SpotData.CanAccommodate(NumMonsters);
}


bool AHidingSpot::CanBeUsedBy(ECreatureSize CreatureSize) const
{
    return !bIsOccupied && static_cast<int32>(CreatureSize) <= static_cast<int32>(MaxCreatureSize);
}

void AHidingSpot::Occupy()
{
	bIsOccupied = true;
}

void AHidingSpot::Vacate()
{
	bIsOccupied = false;
}
