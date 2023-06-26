// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Shoot/ShootTypes/TurningInPlace.h"
#include "Shoot/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Shoot/ShootTypes/CombatState.h"
#include "ShootCharacter.generated.h"

UCLASS()
class SHOOT_API AShootCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:

	AShootCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	void PlayReloadMontage();
	void PlayThrowGrenadeMontage();

	virtual void OnRep_ReplicatedMovement() override;

	void Elim(); // called only on server.
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	// This is replicated.
	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
protected:

	virtual void BeginPlay() override;


	// Poll for any relevant classes and initialize our HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	virtual void Jump() override;

	void EquipButtonPressed();
	void CrouchButtonPressed();
	void CrouchButtonReleased();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();
	void GrenadeButtonPressed();

	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();

	void PlayHitReactMontage();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);  // Rep notify do not call on the server! And its parameter can only be the one who will be replicated

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed(); // 客户端告诉服务端做什么

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	/**
	 * Animation Montages
	 */
	UPROPERTY(EditAnywhere, Category = combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = combat)
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = combat)
	UAnimMontage* ThrowGrenadeMontage;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	UPROPERTY(EditAnywhere, Category = combat)
	float TurnThreshold = 2.f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	/**
	 * Player Health
	 */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	/**
	 * Player Shield
	 */

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 100.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);


	UPROPERTY()
	class AShootPlayerController* ShootPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;;
	void ElimTimerFinished();

	/**
	 * Dissolve Effect
	 */
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	// Dynamic instance that we can change at runtime.
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance0;

	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance1;
	// Material instance set on the Blueprint, used with the dynamic material instance.
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance0;
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance1;

	/**
	 * Elim Bot
	 */
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	UPROPERTY()
	class AShootPlayerState* ShootPlayerState;

	/**
	 * Grenade
	 */
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;
public:	

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; };
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; };
	AWeapon* GetEuqippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
};
