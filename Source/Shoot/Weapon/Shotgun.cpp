// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Shoot/Character/ShootCharacter.h"
#include "Shoot/PlayerController/ShootPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shoot/ShootComponents/LagCompensationComponent.h"


void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		TMap<AShootCharacter*, uint32> HitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			AShootCharacter* ShootCharacter = Cast<AShootCharacter>(FireHit.GetActor());
			if (ShootCharacter)
			{
				if (HitMap.Contains(ShootCharacter))
				{
					HitMap[ShootCharacter]++;
				}
				else
				{
					HitMap.Emplace(ShootCharacter, 1);
				}
			}

			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-0.5, 0.5f)
				);
			}
		}

		TArray<AShootCharacter*> HitCharacters;

		for (auto HitPair : HitMap)
		{
			if (HitPair.Key  && InstigatorController)
			{
				if (HasAuthority() && !bUseServerSideRewind)
				{
					UGameplayStatics::ApplyDamage(
						HitPair.Key,
						HitPair.Value * Damage,
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}

				HitCharacters.Add(HitPair.Key);
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			ShootOwnerCharacter = ShootOwnerCharacter == nullptr ? Cast<AShootCharacter>(OwnerPawn) : ShootOwnerCharacter;
			ShootOwnerController = ShootOwnerController == nullptr ? Cast<AShootPlayerController>(InstigatorController) : ShootOwnerController;
			if (ShootOwnerController && ShootOwnerCharacter && ShootOwnerCharacter->GetLagCompensation() && ShootOwnerCharacter->IsLocallyControlled())
			{
				ShootOwnerCharacter->GetLagCompensation()->ShotServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					ShootOwnerController->GetServerTime() - ShootOwnerController->SingleTripTime
				);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return;

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		ToEndLoc = TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size();

		HitTargets.Add(ToEndLoc);
	}
}
