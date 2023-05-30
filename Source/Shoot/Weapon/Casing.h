// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class SHOOT_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	

	ACasing();

protected:

	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;

public:	
	virtual void Tick(float DeltaTime) override;

};
