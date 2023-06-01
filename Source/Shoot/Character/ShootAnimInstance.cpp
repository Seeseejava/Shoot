// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootAnimInstance.h"
#include "ShootCharacter.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shoot/Weapon/Weapon.h"

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
	bWeaponEquipped = ShootCharacter->IsWeaponEquipped();// 这里需要EquippedWeapon能够复制
	EquippedWeapon = ShootCharacter->GetEuqippedWeapon();
	bIsCrouched = ShootCharacter->bIsCrouched; // 这个自动复制了
	bAiming = ShootCharacter->IsAiming();
	TurningInPlace = ShootCharacter->GetTurningInPlace();

	// Offset Yaw for Strafing
	FRotator AimRotation = ShootCharacter->GetBaseAimRotation(); // global 这是一个可复制的值
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShootCharacter->GetVelocity());
	// YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	// 为了解决插值之后倒退动画会出现从180到-180的问题
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f); // 会寻找最短路径，即不会从-180到0到180
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShootCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime; // magnify

	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = ShootCharacter->GetAO_Yaw();
	AO_Pitch = ShootCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && ShootCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		ShootCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		// it is not important for simulateProxy to update the weapon direction
		if(ShootCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTranform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTranform.GetLocation(), RightHandTranform.GetLocation() + (RightHandTranform.GetLocation() - ShootCharacter->GetHitTarget()));
		}


		//FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
		//FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000, FColor::Red);
		//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), ShootCharacter->GetHitTarget(), FColor::Orange);
	}
}
