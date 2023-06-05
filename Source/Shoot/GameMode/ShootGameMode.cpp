// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootGameMode.h"
#include "Shoot/Character/ShootCharacter.h"
#include "Shoot/PlayerController/ShootPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Shoot/PlayerState/ShootPlayerState.h"

void AShootGameMode::PlayerEliminated(class AShootCharacter* ElimmedCharacter, class AShootPlayerController* VictimController, AShootPlayerController* AttackerController)
{
	AShootPlayerState* AttackerPlayerState = AttackerController ? Cast<AShootPlayerState>(AttackerController->PlayerState) : nullptr;
	AShootPlayerState* VictimPlayerState = VictimController ? Cast<AShootPlayerState>(VictimController->PlayerState) : nullptr;
	
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void AShootGameMode::RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
