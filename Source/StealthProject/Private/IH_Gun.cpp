// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Gun.h"
#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>
#include "TPSPlayer.h"
#include "PlayerAnim.h"
#include <Components/WidgetComponent.h>
#include <Blueprint/UserWidget.h>
#include <Components/SphereComponent.h>

// Sets default values
AIH_Gun::AIH_Gun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(compBox);
	compBox -> SetBoxExtent(FVector(50));

	compMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	compMesh -> SetupAttachment(compBox);
	compMesh -> SetRelativeLocation(FVector(0, -10, 0));
	compMesh -> SetRelativeRotation(FRotator(-90, 0, 0));
	compMesh->SetRelativeScale3D(FVector(1.5));
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Pistols_A.Pistols_A'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetSkeletalMesh(tempMesh.Object);
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
void AIH_Gun::BeginPlay()
{
	Super::BeginPlay();

//	compBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Gun::OnOverlap);
	compWidget->SetVisibility(false);
	compSphere->OnComponentBeginOverlap.AddDynamic(this, &AIH_Gun::OnSphereOverlap);
	compSphere->OnComponentEndOverlap.AddDynamic(this, &AIH_Gun::OnSphereEndOverlap);
}

// Called every frame
void AIH_Gun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// void AIH_Gun::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	character = Cast<ATPSPlayer>(OtherActor);
// 	
// 	if (character != nullptr)
// 	{
// 		UE_LOG(LogTemp, Warning, (TEXT("Get Gun!")));
// 		character->bgetGun=true;  // bgetGun이 true일 때만 2번 키를 누를 수 있음.
// 		character->anim->isGunEquipped=true;
// 		character->ChangeToPistol();
// 		Destroy();
// 	}
// 	else return;
// }

void AIH_Gun::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(true);
	}
}

void AIH_Gun::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(false);
	}
}