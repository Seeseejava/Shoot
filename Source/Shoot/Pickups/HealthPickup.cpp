// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "Shoot/Character/ShootCharacter.h"
#include "Shoot/ShootComponents/BuffComponent.h"


AHealthPickup::AHealthPickup()
{
	bReplicates = true;
	
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AShootCharacter* ShootCharacter = Cast<AShootCharacter>(OtherActor);
	if (ShootCharacter)
	{
		UBuffComponent* Buff = ShootCharacter->GetBuff();
		if (Buff)
		{
			Buff->Heal(HealAmount, HealingTime);
		}
	}
	Destroy();
}



