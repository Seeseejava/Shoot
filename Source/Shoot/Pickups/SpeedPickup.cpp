// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedPickup.h"
#include "Shoot/Character/ShootCharacter.h"
#include "Shoot/ShootComponents/BuffComponent.h"

void ASpeedPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AShootCharacter* ShootCharacter = Cast<AShootCharacter>(OtherActor);
	if (ShootCharacter)
	{
		UBuffComponent* Buff = ShootCharacter->GetBuff();
		if (Buff)
		{
			Buff->BuffSpeed(BaseSpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
		}
	}
	Destroy();
}