// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketMovementComponent.h"

UProjectileMovementComponent::EHandleBlockingHitResult URocketMovementComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
	Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void URocketMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice /*= 0.f*/, const FVector& MoveDelta /*= FVector::ZeroVector*/)
{
	// Rockets should not stop; only explode when their collisionBox detects a hit.
}
