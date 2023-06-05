// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShootPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API AShootPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void OnRep_Score() override;
	void AddtoScore(float ScoreAmount);
private:
	class AShootCharacter* Character;
	class AShootPlayerController* Controller;
};
