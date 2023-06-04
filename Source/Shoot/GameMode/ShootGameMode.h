// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShootGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API AShootGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void PlayerEliminated(class AShootCharacter* ElimmedCharacter, class AShootPlayerController* VictimController, AShootPlayerController* AttackerController);
};
