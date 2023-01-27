// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class STEALTHPROJECT_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* compMesh;

	UPROPERTY(EditAnywhere)
	float moveSpeed = 300;

	UPROPERTY(EditAnywhere)
	float currentTime = 0;

	UPROPERTY(EditAnywhere)
	float openDelayTime = 1;

	UPROPERTY(EditAnywhere)
	float rotateZ = 0;

	UPROPERTY(EditAnywhere)
	class USoundBase* doorSound;
};
