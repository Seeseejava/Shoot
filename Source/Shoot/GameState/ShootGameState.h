// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShootGameState.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API AShootGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class AShootPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<AShootPlayerState*> TopScoringPlayers;
private:
	float TopScore = 0.f;
};
