// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootAnimInstance.h"
#include "ShootCharacter.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShootAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShootCharacter = Cast<AShootCharacter>(TryGetPawnOwner());
}

void UShootAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	// first frame
	if (ShootCharacter == nullptr)
	{
		ShootCharacter = Cast<AShootCharacter>(TryGetPawnOwner());
	}

	if (ShootCharacter == nullptr) return;

	FVector Velocity = ShootCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = ShootCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = ShootCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = ShootCharacter->IsWeaponEquipped();// ������ҪEquippedWeapon�ܹ�����
	bIsCrouched = ShootCharacter->bIsCrouched; // ����Զ�������
	bAiming = ShootCharacter->IsAiming();

	// Offset Yaw for Strafing
	FRotator AimRotation = ShootCharacter->GetBaseAimRotation(); // global ����һ���ɸ��Ƶ�ֵ
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShootCharacter->GetVelocity());
	// YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	// Ϊ�˽����ֵ֮���˶�������ִ�180��-180������
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f); // ��Ѱ�����·�����������-180��0��180
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShootCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime; // magnify

	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = ShootCharacter->GetAO_Yaw();
	AO_Pitch = ShootCharacter->GetAO_Pitch();
}
