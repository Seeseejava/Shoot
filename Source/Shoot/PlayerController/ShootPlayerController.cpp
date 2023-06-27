// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootPlayerController.h"
#include "Shoot/HUD/ShootHUD.h"
#include "Shoot/HUD/CharacterOverlay.h"
#include "Shoot/HUD/Announcement.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Shoot/Character/ShootCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Shoot/GameMode/ShootGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Shoot/ShootComponents/CombatComponent.h"
#include "Shoot/GameState/ShootGameState.h"
#include "Shoot/PlayerState/ShootPlayerState.h"

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
	else
	{
		// CharacterOverlay not initialize until now
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void AShootPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	bool bHUDValid = ShootHUD &&
		ShootHUD->CharacterOverlay &&
		ShootHUD->CharacterOverlay->ShieldBar &&
		ShootHUD->CharacterOverlay->ShieldText;
	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		ShootHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		ShootHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		// CharacterOverlay not initialize until now
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
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
	else
	{
		bInitializeScore = true;
		HUDScore = Score;
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
	else
	{
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
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
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
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
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
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
		if (CountdownTime < 0.f)
		{
			ShootHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ShootHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AShootPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	bool bHUDValid = ShootHUD &&
		ShootHUD->Announcement &&
		ShootHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			// Hide the HUD when count less than zero.
			ShootHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ShootHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AShootPlayerController::SetHUDGrenades(int32 Grenades)
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	bool bHUDValid = ShootHUD &&
		ShootHUD->CharacterOverlay &&
		ShootHUD->CharacterOverlay->GrenadesText;
	if (bHUDValid)
	{
		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		ShootHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesText));
	}
	else
	{
		bInitializeGrenades = true;
		HUDGrenades = Grenades;
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
	ServerCheckMatchState();
}

void AShootPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();

	CheckTimeSync(DeltaTime);
	PollInit();
}

void AShootPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShootPlayerController, MatchState);
}

void AShootPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AShootPlayerController::ServerCheckMatchState_Implementation()
{
	AShootGameMode* GameMode = Cast<AShootGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();

		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);

	}
}

void AShootPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState); // For this may happed after replicate.
	if (ShootHUD && MatchState == MatchState::WaitingToStart)
	{
		ShootHUD->AddAnnouncement();
	}
}

float AShootPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AShootPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

// called only on the server. Because GameMode calls it.
void AShootPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}



void AShootPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AShootPlayerController::HandleMatchHasStarted()
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	if (ShootHUD)
	{
		if(ShootHUD->CharacterOverlay == nullptr) ShootHUD->AddCharacterOverlay();
		if (ShootHUD->Announcement)
		{
			ShootHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AShootPlayerController::HandleCooldown()
{
	ShootHUD = ShootHUD == nullptr ? Cast<AShootHUD>(GetHUD()) : ShootHUD;
	if (ShootHUD)
	{
		ShootHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = ShootHUD->Announcement &&
			ShootHUD->Announcement->AnnouncementText &&
			ShootHUD->Announcement->InfoText;
		if (bHUDValid)
		{
			ShootHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:");
			ShootHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			AShootGameState* ShootGameState = Cast<AShootGameState>(UGameplayStatics::GetGameState(this));
			AShootPlayerState* ShootPlayerState = GetPlayerState<AShootPlayerState>();
			if (ShootGameState && ShootPlayerState)
			{
				TArray<AShootPlayerState*> TopPlayers = ShootGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == ShootPlayerState)
				{
					InfoTextString = FString("You are the winner.");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win: \n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				ShootHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}

		}
	}
	AShootCharacter* ShootCharacter = Cast<AShootCharacter>(GetPawn());
	if (ShootCharacter && ShootCharacter->GetCombat())
	{
		ShootCharacter->bDisableGameplay = true;
		ShootCharacter->GetCombat()->FireButtonPressed(false);
	}
}

void AShootPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (ShootHUD && ShootHUD->CharacterOverlay)
		{
			CharacterOverlay = ShootHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeHealth)      SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield)      SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore)       SetHUDScore(HUDScore);
				if (bInitializeDefeats)     SetHUDDefeats(HUDDefeats);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo)  SetHUDWeaponAmmo(HUDWeaponAmmo);
				AShootCharacter* ShootCharacter = Cast<AShootCharacter>(GetPawn());
				if (ShootCharacter && ShootCharacter->GetCombat())
				{
					if (bInitializeGrenades) SetHUDGrenades(ShootCharacter->GetCombat()->GetGrenades());
				}
			}
		}
	}
}

void AShootPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - (GetServerTime() - LevelStartingTime);
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - (GetServerTime() - LevelStartingTime);
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - (GetServerTime() - LevelStartingTime);

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (HasAuthority())
	{
		ShootGameMode = ShootGameMode == nullptr ? Cast<AShootGameMode>(UGameplayStatics::GetGameMode(this)) : ShootGameMode;
		if (ShootGameMode)
		{
			SecondsLeft = FMath::CeilToInt(ShootGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void AShootPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimerOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimerOfReceipt);
}

void AShootPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

