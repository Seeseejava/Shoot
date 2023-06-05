// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootPlayerState.h"
#include "Shoot/Character/ShootCharacter.h"
#include "Shoot/PlayerController/ShootPlayerController.h"

void AShootPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<AShootCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShootPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}
}

void AShootPlayerState::AddtoScore(float ScoreAmount)
{
	Score += ScoreAmount;

	Character = Character == nullptr ? Cast<AShootCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShootPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}
}
