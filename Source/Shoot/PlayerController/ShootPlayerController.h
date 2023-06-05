// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShootPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API AShootPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	virtual void OnPossess(APawn* InPawn) override;
protected:
	virtual void BeginPlay() override;
private:
	class AShootHUD* ShootHUD;
	
};
