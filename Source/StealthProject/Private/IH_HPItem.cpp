// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_HPItem.h"
#include <Components/BoxComponent.h>
#include "TPSPlayer.h"

// Sets default values
AIH_HPItem::AIH_HPItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(compBox);
	compBox->SetBoxExtent(FVector(50));

	compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	compMesh->SetupAttachment(compBox);
	compMesh->SetRelativeLocation(FVector(0, 0, -30));
	compMesh->SetRelativeScale3D(FVector(0.5));
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void AIH_HPItem::BeginPlay()
{
	Super::BeginPlay();

	compMesh->OnComponentBeginOverlap.AddDynamic(this, &AIH_HPItem::OnOverlap);
}

// Called every frame
void AIH_HPItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIH_HPItem::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		character->HP+=5;
		UE_LOG(LogTemp, Warning, TEXT("Current HP : %d"), character->HP);

		Destroy();
	}
}