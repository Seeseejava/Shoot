// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootGameMode.h"
#include "Shoot/Character/ShootCharacter.h"
#include "Shoot/PlayerController/ShootPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Shoot/PlayerState/ShootPlayerState.h"
#include "Shoot/GameState/ShootGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AShootGameMode::AShootGameMode()
{
	bDelayedStart = true;
}

void AShootGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Time of joining in the ShootingMap
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AShootGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - (GetWorld()->GetTimeSeconds() - LevelStartingTime);
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - (GetWorld()->GetTimeSeconds() - LevelStartingTime);
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - (GetWorld()->GetTimeSeconds() - LevelStartingTime);
		if (CountdownTime < 0.f)
		{
			RestartGame();
		}
	}
}

void AShootGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AShootPlayerController* ShootPlayer = Cast<AShootPlayerController>(*It);
		if (ShootPlayer)
		{
			ShootPlayer->OnMatchStateSet(MatchState);
		}
	}
}

void AShootGameMode::PlayerEliminated(class AShootCharacter* ElimmedCharacter, class AShootPlayerController* VictimController, AShootPlayerController* AttackerController)
{
	AShootPlayerState* AttackerPlayerState = AttackerController ? Cast<AShootPlayerState>(AttackerController->PlayerState) : nullptr;
	AShootPlayerState* VictimPlayerState = VictimController ? Cast<AShootPlayerState>(VictimController->PlayerState) : nullptr;
	
	AShootGameState* ShootGameState = GetGameState<AShootGameState>();
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && ShootGameState)
	{
		AttackerPlayerState->AddToScore(1.f);
		ShootGameState->UpdateTopScore(AttackerPlayerState);
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
