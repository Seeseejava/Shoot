// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

		UPROPERTY()
		FVector Location;

	UPROPERTY()
		FRotator Rotation;

	UPROPERTY()
		FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;

	UPROPERTY()
	AShootCharacter* Character;
};


USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed = false;

	UPROPERTY()
	bool bHeadShot = false;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<AShootCharacter*, uint32> HeadShots;
	UPROPERTY()
	TMap<AShootCharacter*, uint32> BodyShots;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOT_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	ULagCompensationComponent();
	friend class AShootCharacter;
	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);
	
	/**
	* Hitscan 
	*/
	FServerSideRewindResult ServerSideRewind(
		class AShootCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart, 
		const FVector_NetQuantize& HitLocation, 
		float HitTime
	);

	/**
	 * Shotgun
	 */
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<AShootCharacter*>& HitHCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);

	/**
	 * Projectile
	 */
	FServerSideRewindResult ProjectileServerSideRewind(
		AShootCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);


	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		AShootCharacter* HitCharacter, 
		const FVector_NetQuantize& TraceStart, 
		const FVector_NetQuantize& HitLocation, 
		float HitTime, 
		class AWeapon* DamageCauser
	);
	UFUNCTION(Server, Reliable)
	void ShotServerScoreRequest(
		const TArray<AShootCharacter*>& HitHCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);
	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(
		AShootCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);
protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	
	void CacheBoxPositions(AShootCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(AShootCharacter* HitCharacter, const FFramePackage& Package);
	void ResetBoxes(AShootCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(AShootCharacter* HitCharacter, ECollisionEnabled:: Type CollisionEnabled);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(AShootCharacter* HitCharacter, float HitTime);
	
	/**
	* HitScan
	*/
	FServerSideRewindResult ConfirmHit(
		const FFramePackage& Package, 
		AShootCharacter* HitCharacter, 
		const FVector_NetQuantize& TraceStart, 
		const FVector_NetQuantize& HitLocation
	);

	/**
	 * Shotgun
	 */
	FShotgunServerSideRewindResult ShotgunConfirmHit(
		const TArray<FFramePackage>& FFramePackages,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations
	);

	/**
	 * Projectile
	 */
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage& Package,
		AShootCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

private:

	UPROPERTY()
	AShootCharacter* Character;
	UPROPERTY()
	class AShootPlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
