// Fill out your copyright notice in the Description page of Project Settings.


#include "Kal.h"
#include "TPSPlayer.h"
#include <Components/BoxComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "PlayerAnim.h"
#include <Components/SphereComponent.h>
#include <Components/WidgetComponent.h>
#include <Blueprint/UserWidget.h>

// Sets default values
AKal::AKal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(compBox);
	compBox->SetBoxExtent(FVector(50));
	compMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Kal"));
	compMesh->SetupAttachment(compBox);
	compMesh->SetRelativeRotation(FRotator(-90, 0, 0));
	compMesh->SetRelativeScale3D(FVector(1.5));
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Knife_A.Knife_A'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetSkeletalMesh(tempMesh.Object);
	}

	compSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	compSphere->SetupAttachment(RootComponent);
	compSphere->SetSphereRadius(450);

	compWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pointer"));
	compWidget->SetupAttachment(RootComponent);
	compWidget->SetRelativeLocation(FVector(15, 0, 30));

	ConstructorHelpers::FClassFinder<UUserWidget> tempWidget(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_Pointer.WG_Pointer_C'"));
	if (tempWidget.Succeeded())
	{
		compWidget->SetWidgetClass(tempWidget.Class);
		compWidget->SetWidgetSpace(EWidgetSpace::Screen);
		compWidget->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void AKal::BeginPlay()
{
	Super::BeginPlay();

	compBox->OnComponentBeginOverlap.AddDynamic(this, &AKal::OnOverlap);
	compBox->OnComponentEndOverlap.AddDynamic(this, &AKal::OnEndOverlap);
	compSphere->OnComponentBeginOverlap.AddDynamic(this, &AKal::OnSphereOverlap);
	compSphere->OnComponentEndOverlap.AddDynamic(this, &AKal::OnSphereEndOverlap);
}

// Called every frame
void AKal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKal::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);    // 오버랩된 플레이어를 캐스팅

	if (character != nullptr)    // 널이 아니라면 
	{
		character->pickKnife=this;
// 		character->bgetKal = true;      //  칼여부 = true, true일 때만 3번 키를 누를 수 있음.
// 		character->ChangeToKal();   //  칼 함수호출
// 		character->anim->isKal = true;  //   칼 애님 = true
// 		character->anim->isGunEquipped = false;  // 총애님 no
// 
// 		Destroy();
	}
}

void AKal::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		character->pickKnife = nullptr;
	}
}

void AKal::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(true);
	}
}

void AKal::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(false);
	}
}