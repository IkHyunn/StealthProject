// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "../StealthProjectGameModeBase.h"
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/Wise/Resources/StorageHouse/Meshes/Buildings/SM_Door_01.SM_Door_01'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetStaticMesh(tempMesh.Object);
	}

	rotateZ = -90;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();
	
	if (currMode->currScore == 5)  // 현재 점수가 일정 이상이 되면
	{
		if (rotateZ <= 0)
		{
			SetActorRotation(FRotator(0, rotateZ, 0));
			rotateZ += DeltaTime * 200;
		}
	}
}
