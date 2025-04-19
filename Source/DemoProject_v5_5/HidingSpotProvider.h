#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HidingSpotProvider.generated.h"

// Esta é a interface que os objetos que fornecem esconderijos devem implementar
UINTERFACE(NotBlueprintable)
class UHidingSpotProvider : public UInterface
{
	GENERATED_BODY()
};

class DEMOPROJECT_V5_5_API IHidingSpotProvider
{
	GENERATED_BODY()

public:
	// Método para obter a localização do ponto de esconderijo
	UFUNCTION(BlueprintCallable, Category = "HidingSpot")
	virtual FVector GetHidingSpotLocation() const;

	// Método para verificar se o monstro pode se esconder nesse local
	UFUNCTION(BlueprintCallable, Category = "HidingSpot")
	virtual bool CanMonsterHideHere() const;
};
