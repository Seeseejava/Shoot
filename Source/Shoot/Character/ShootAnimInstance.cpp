// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootAnimInstance.h"
#include "ShootCharacter.h"
#include "GameFramework\CharacterMovementComponent.h"

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
	bIsCrouched = ShootCharacter->bIsCrouched; // 这个自动复制了
}
