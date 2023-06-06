// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootPlayerController.h"
#include "Shoot/HUD/ShootHUD.h"
#include "Shoot/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Shoot/Character/ShootCharacter.h"

void AShootPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	bool bHUDValid = ShootHUD && 
		ShootHUD->CharacterOverlay && 
		ShootHUD->CharacterOverlay->HealthBar && 
		ShootHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		ShootHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		ShootHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AShootPlayerController::SetHUDScore(float Score)
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	bool bHUDValid = ShootHUD &&
		ShootHUD->CharacterOverlay &&
		ShootHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		ShootHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void AShootPlayerController::SetHUDDefeats(int32 Defeats)
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	bool bHUDValid = ShootHUD &&
		ShootHUD->CharacterOverlay &&
		ShootHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		ShootHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void AShootPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	bool bHUDValid = ShootHUD &&
		ShootHUD->CharacterOverlay &&
		ShootHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString WeaponAmmoText = FString::Printf(TEXT("%d"), Ammo);
		ShootHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoText));
	}
}

void AShootPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	bool bHUDValid = ShootHUD &&
		ShootHUD->CharacterOverlay &&
		ShootHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString carriedAmmoText = FString::Printf(TEXT("%d"), Ammo);
		ShootHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(carriedAmmoText));
	}
}

void AShootPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	bool bHUDValid = ShootHUD &&
		ShootHUD->CharacterOverlay &&
		ShootHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ShootHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AShootPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AShootCharacter* ShootCharacter = Cast<AShootCharacter>(InPawn);
	if (ShootCharacter)
	{
		// Because SetHUDHealth before, the cast may unsuccess.
		SetHUDHealth(ShootCharacter->GetHealth(), ShootCharacter->GetMaxHealth());
	}
}

void AShootPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ShootHUD = Cast<AShootHUD>(GetHUD());
}

void AShootPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
}


void AShootPlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetWorld()->GetTimeSeconds());
	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetWorld()->GetTimeSeconds());
	}
	CountdownInt = SecondsLeft;
}
