// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Shoot/Weapon/Weapon.h"
#include "Shoot/Character/ShootCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	BaseWalkSpeed = 600;
	AimWalkSpeed = 450;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
 	}
}


void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;  // ���û����仰��������Ҫ�ȴ�RPC���������
	// һ����Ч������ΪServerSetAiming����server��ִ��
	//if (!Character->HasAuthority())
	//{
	//	ServerSetAiming(bIsAiming);
	//}
	ServerSetAiming(bIsAiming);

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (EquippedWeapon == nullptr) return;
	if (Character && bFireButtonPressed)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire();
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);    // ��������owner�ǿɸ��Ƶģ����Կͻ��˲���Ҫ�������
	// EquippedWeapon->ShowPickupWidget(false);// ����ֻ��Ҫ�ڿͻ��������������,����Ѻ�������WeaponState������
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

}

