// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Bullet.h"
#include <Components/BoxComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "TPSPlayer.h"
#include <Components/SphereComponent.h>
#include <Blueprint/UserWidget.h>
#include <Components/WidgetComponent.h>

// Sets default values
AIH_Bullet::AIH_Bullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX"));
	SetRootComponent(compBox);
	compBox->SetBoxExtent(FVector(50));

	compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupBullet"));
	compMesh->SetupAttachment(compBox);
	compMesh->SetRelativeScale3D(FVector(1.5));
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/Wise/Resources/MilitaryWeapSilver/Pickups/Pistol_Pickup.Pistol_Pickup'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetStaticMesh(tempMesh.Object);
	}

	compSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	compSphere->SetupAttachment(RootComponent);
	compSphere->SetSphereRadius(450);

	compWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pointer"));
	compWidget->SetupAttachment(RootComponent);
	compWidget->SetRelativeLocation(FVector(15, 0, 60));

	ConstructorHelpers::FClassFinder<UUserWidget> tempWidget(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_Pointer.WBP_Pointer_C'"));
	if (tempWidget.Succeeded())
	{
		compWidget->SetWidgetClass(tempWidget.Class);
		compWidget->SetWidgetSpace(EWidgetSpace::Screen);
	}
}

// Called when the game starts or when spawned
void AIH_Bullet::BeginPlay()
{
	Super::BeginPlay();
	
	compBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Bullet::OnOverlap);
	compWidget->SetVisibility(false);
	compSphere->OnComponentBeginOverlap.AddDynamic(this, &AIH_Bullet::OnSphereOverlap);
	compSphere->OnComponentEndOverlap.AddDynamic(this, &AIH_Bullet::OnSphereEndOverlap);
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
		character->currentBullet+=plusBullet;
		UE_LOG(LogTemp, Warning, TEXT("Current Bullet : %d"), character->currentBullet);

		Destroy();
	}
}

void AIH_Bullet::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(true);
	}
}

void AIH_Bullet::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(false);
	}
}