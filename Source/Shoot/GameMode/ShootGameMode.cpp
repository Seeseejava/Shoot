// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootGameMode.h"
#include "Shoot/Character/ShootCharacter.h"
#include "Shoot/PlayerController/ShootPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void AShootGameMode::PlayerEliminated(class AShootCharacter* ElimmedCharacter, class AShootPlayerController* VictimController, AShootPlayerController* AttackerController)
{
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