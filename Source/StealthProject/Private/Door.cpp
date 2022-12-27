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
	
	if (currMode->currScore == 1)  // ���� ������ ���� �̻��� �Ǹ�
	{
		compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // compMesh�� Collision ������ �������

		FVector P0 = GetActorLocation();
		FVector Vt = FVector::DownVector * moveSpeed * DeltaTime;
		FVector P = P0 + Vt;  // ���� ���� ������ ��������.

		SetActorLocation(P);

		if (P.Z < -200.0f)  // Z ��ġ���� -200 ���ϰ� �Ǹ�
		{
			Destroy();  // �ı��ȴ�.
		}
	}
}
