// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Knife.h"
#include <Components/BoxComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "TPSPlayer.h"

// Sets default values
AIH_Knife::AIH_Knife()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(compBox);

	compMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Knife"));
	compMesh -> SetupAttachment(compBox);
	compMesh->SetRelativeLocation(FVector(0, -5, -5));
	compMesh->SetRelativeScale3D(FVector(1.5));
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Knife_A.Knife_A'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetSkeletalMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void AIH_Knife::BeginPlay()
{
	Super::BeginPlay();

	compBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Knife::OnOverlap);
}

// Called every frame
void AIH_Knife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIH_Knife::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup Knife"));
		Destroy();
	}
}