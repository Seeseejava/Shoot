// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootCharacter.h"
#include "GameFramework\SpringArmComponent.h"
#include "Camera\CameraComponent.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shoot/Weapon/Weapon.h"
#include "Shoot/ShootComponents/CombatComponent.h"
#include "Shoot/ShootComponents/BuffComponent.h"
#include "Components\CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ShootAnimInstance.h"
#include "Shoot/PlayerController/ShootPlayerController.h"
#include "Shoot/GameMode/ShootGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Shoot/PlayerState/ShootPlayerState.h"
#include "Shoot/Weapon/WeaponTypes.h"

AShootCharacter::AShootCharacter()
{
 
	PrimaryActorTick.bCanEverTick = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom)"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollwCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);  // This Component is repliated itself and it does not need to be register

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true); 

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	// do not block camera
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); 
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AShootCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShootCharacter, OverlappingWeapon, COND_OwnerOnly); // need include  "Net/UnrealNetwork.h"
	DOREPLIFETIME(AShootCharacter, Health);
	DOREPLIFETIME(AShootCharacter, bDisableGameplay);
}


void AShootCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
}

void AShootCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
	if (AnimInstace && FireWeaponMontage)
	{
		AnimInstace->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstace->Montage_JumpToSection(SectionName);
	}
}

void AShootCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
	if (AnimInstace && ElimMontage)
	{
		AnimInstace->Montage_Play(ElimMontage);
	}
}

void AShootCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
	if (AnimInstace && ReloadMontage)
	{
		AnimInstace->Montage_Play(ReloadMontage);

		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SMG:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_ShotGun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("Rifle");
			break;
		}
		AnimInstace->Montage_JumpToSection(SectionName);
	}
}

void AShootCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
	if (AnimInstace && ThrowGrenadeMontage)
	{
		AnimInstace->Montage_Play(ThrowGrenadeMontage);
	}
}

void AShootCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
	if (AnimInstace && HitReactMontage)
	{
		AnimInstace->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");

		AnimInstace->Montage_JumpToSection(SectionName);
	}
}

void AShootCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimmed) return;
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	UpdateHUDHealth();
	PlayHitReactMontage();

	if(Health == 0.f)
	{
		AShootGameMode* ShootGameMode = GetWorld()->GetAuthGameMode<AShootGameMode>();
		if (ShootGameMode)
		{
			ShootPlayerController = ShootPlayerController == nullptr ? Cast<AShootPlayerController>(Controller) : ShootPlayerController;
			AShootPlayerController* AttackController = Cast<AShootPlayerController>(InstigatorController);
			ShootGameMode->PlayerEliminated(this, ShootPlayerController, AttackController);
		}
	}
}

void AShootCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();
	
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AShootCharacter::ReceiveDamage);
	}
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
}

void AShootCharacter::UpdateHUDHealth()
{
	ShootPlayerController = ShootPlayerController == nullptr ? Cast<AShootPlayerController>(Controller) : ShootPlayerController;
	if (ShootPlayerController)
	{
		ShootPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AShootCharacter::PollInit()
{
	if (ShootPlayerState == nullptr)
	{
		// This will return nullptr in frame 0.
		ShootPlayerState = GetPlayerState<AShootPlayerState>();
		if (ShootPlayerState)
		{
			ShootPlayerState->AddToScore(0.f);
			ShootPlayerState->AddToDefeats(0);
		}
	}
}

void AShootCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);

	HideCameraIfCharacterClose();
	PollInit();
}

void AShootCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.1f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}



void AShootCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShootCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AShootCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShootCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShootCharacter::CrouchButtonReleased);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AShootCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AShootCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShootCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShootCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShootCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AShootCharacter::GrenadeButtonPressed);


	PlayerInputComponent->BindAxis("MoveForward", this, &AShootCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShootCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AShootCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShootCharacter::LookUp);

}



void AShootCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AShootCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AShootCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AShootCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AShootCharacter::Jump()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AShootCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void AShootCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	Crouch();
}

void AShootCharacter::CrouchButtonReleased()
{
	if (bDisableGameplay) return;
	UnCrouch();
}

void AShootCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AShootCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void AShootCharacter::FireButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AShootCharacter::FireButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AShootCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->Reload();
	}
}

void AShootCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

void AShootCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping, thus we will consider yawoffset
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.0f || bIsInAir) // running, or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void AShootCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch; // 因为网络传输会压缩pitch，导致负数变成正数
	if (AO_Pitch > 90.0f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutInRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutInRange, AO_Pitch);
	}
}

void AShootCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	// UE_LOG(LogTemp, Warning, TEXT("ProxyYaw: %f"), ProxyYaw);
	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
	}
	else
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
}

void AShootCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}

}

void AShootCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void AShootCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}


void AShootCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	// Match the Newwork update speed.
	SimProxiesTurn();

	TimeSinceLastMovementReplication = 0.f;
}

void AShootCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&AShootCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void AShootCharacter::MulticastElim_Implementation()
{
	if (ShootPlayerController)
	{
		ShootPlayerController->SetHUDWeaponAmmo(0);
	}

	bElimmed = true;
	PlayElimMontage();

	// Start dissolve effect
	if (DissolveMaterialInstance0)
	{
		DynamicDissolveMaterialInstance0 = UMaterialInstanceDynamic::Create(DissolveMaterialInstance0, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance0);
		DynamicDissolveMaterialInstance0->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance0->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	if (DissolveMaterialInstance1)
	{
		DynamicDissolveMaterialInstance1 = UMaterialInstanceDynamic::Create(DissolveMaterialInstance1, this);
		GetMesh()->SetMaterial(1, DynamicDissolveMaterialInstance0);
		DynamicDissolveMaterialInstance1->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance1->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();

	// Disable character movement
	GetCharacterMovement()->DisableMovement();          // For moving
	GetCharacterMovement()->StopMovementImmediately();	// For Rotating
	bDisableGameplay = true;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn elim bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 150.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation()
		);
	}
	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimBotSound,
			GetActorLocation()
		);
	}
	bool bHideSniperScope = IsLocallyControlled() &&
		Combat &&
		Combat->bAiming &&
		Combat->EquippedWeapon &&
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
}

void AShootCharacter::Destroyed()
{
	Super::Destroyed();
	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	AShootGameMode* ShootGameMode = Cast<AShootGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = ShootGameMode && ShootGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void AShootCharacter::ElimTimerFinished()
{
	AShootGameMode* ShootGameMode = GetWorld()->GetAuthGameMode<AShootGameMode>();
	if (ShootGameMode)
	{
		ShootGameMode->RequestRespawn(this, Controller);
	}
	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}
}

void AShootCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance0)
	{
		DynamicDissolveMaterialInstance0->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
	if (DynamicDissolveMaterialInstance1)
	{
		DynamicDissolveMaterialInstance1->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void AShootCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &AShootCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void AShootCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

float AShootCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void AShootCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}



void AShootCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool AShootCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AShootCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* AShootCharacter::GetEuqippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;

}

FVector AShootCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState AShootCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

