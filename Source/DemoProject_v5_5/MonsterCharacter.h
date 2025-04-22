// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterCharacter.generated.h"


UCLASS()
class DEMOPROJECT_V5_5_API AMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float Health;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
