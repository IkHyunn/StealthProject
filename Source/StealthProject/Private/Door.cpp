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

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetStaticMesh(tempMesh.Object);
	}
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
	
	if (currMode->currScore == 1)  // 현재 점수가 일정 이상이 되면
	{
		compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // compMesh의 Collision 설정이 사라지고

		FVector P0 = GetActorLocation();
		FVector Vt = FVector::DownVector * moveSpeed * DeltaTime;
		FVector P = P0 + Vt;  // 문이 점점 밑으로 내려간다.

		SetActorLocation(P);

		if (P.Z < -200.0f)  // Z 위치값이 -200 이하가 되면
		{
			Destroy();  // 파괴된다.
		}
	}
}
