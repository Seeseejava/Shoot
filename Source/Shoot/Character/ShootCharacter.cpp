// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootCharacter.h"
#include "GameFramework\SpringArmComponent.h"
#include "Camera\CameraComponent.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shoot/Weapon/Weapon.h"
#include "Shoot/ShootComponents/CombatComponent.h"


AShootCharacter::AShootCharacter()
{
 
	PrimaryActorTick.bCanEverTick = true;

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

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AShootCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShootCharacter, OverlappingWeapon, COND_OwnerOnly); // need include  "Net/UnrealNetwork.h"
}


void AShootCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

void AShootCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShootCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShootCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AShootCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShootCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShootCharacter::CrouchButtonReleased);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AShootCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AShootCharacter::AimButtonReleased);


	PlayerInputComponent->BindAxis("MoveForward", this, &AShootCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShootCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AShootCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShootCharacter::LookUp);

}



void AShootCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AShootCharacter::MoveRight(float Value)
{
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

void AShootCharacter::EquipButtonPressed()
{
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
	Crouch();
}

void AShootCharacter::CrouchButtonReleased()
{
	UnCrouch();
}

void AShootCharacter::AimButtonPressed()
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AShootCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
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


