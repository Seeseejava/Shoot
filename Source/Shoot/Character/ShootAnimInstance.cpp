// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootAnimInstance.h"
#include "ShootCharacter.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shoot/Weapon/Weapon.h"
#include "Shoot/ShootTypes/CombatState.h"

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
	EquippedWeapon = ShootCharacter->GetEquippedWeapon();
	bIsCrouched = ShootCharacter->bIsCrouched; // 这个自动复制了
	bAiming = ShootCharacter->IsAiming();
	TurningInPlace = ShootCharacter->GetTurningInPlace();
	brotateRootBone = ShootCharacter->ShouldRotateBone();
	bElimmed = ShootCharacter->IsElimmed();

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
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - ShootCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
		}


		FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000, FColor::Red);
		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), ShootCharacter->GetHitTarget(), FColor::Orange);
	}

	bUseFABRIK = ShootCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
	// 解决换弹时combatstate还没复制过来倒是使用了FABRIK的问题
	if (ShootCharacter->IsLocallyControlled() && ShootCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade)
	{
		bUseFABRIK = !ShootCharacter->IsLocallyControlled();
	}
	bUseAimOffsets = ShootCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !ShootCharacter->GetDisableGameplay();
	bTransformRightHand = ShootCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !ShootCharacter->GetDisableGameplay();

}
