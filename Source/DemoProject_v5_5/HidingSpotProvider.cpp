#include "HidingSpotProvider.h"



// Implementação das funções da interface
FVector IHidingSpotProvider::GetHidingSpotLocation() const
{
	// Implemente a lógica adequada ou lance um erro se for puramente abstrata
	ensureMsgf(false, TEXT("GetHidingSpotLocation not implemented"));
	return FVector::ZeroVector;
}

bool IHidingSpotProvider::CanMonsterHideHere() const
{
	// Implemente a lógica adequada ou lance um erro se for puramente abstrata
	ensureMsgf(false, TEXT("CanMonsterHideHere not implemented"));
	return false;
}