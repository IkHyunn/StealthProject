// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Bullet.h"
#include <Components/BoxComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "TPSPlayer.h"

// Sets default values
AIH_Bullet::AIH_Bullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX"));
	SetRootComponent(compBox);

	compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupBullet"));
	compMesh->SetupAttachment(compBox);
	compMesh->SetRelativeScale3D(FVector(1.5));
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/Wise/Resources/MilitaryWeapSilver/Pickups/Pistol_Pickup.Pistol_Pickup'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void AIH_Bullet::BeginPlay()
{
	Super::BeginPlay();
	
	compBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Bullet::OnOverlap);
}

// Called every frame
void AIH_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIH_Bullet::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		character->currentBullet+=3;
		UE_LOG(LogTemp, Warning, TEXT("Current Bullet : %d"), character->currentBullet);

		Destroy();
	}
}