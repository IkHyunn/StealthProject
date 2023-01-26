// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Knife.h"
#include <Components/BoxComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "TPSPlayer.h"
#include "PlayerAnim.h"
#include <Components/SphereComponent.h>
#include <Components/WidgetComponent.h>
#include <Blueprint/UserWidget.h>

// Sets default values
AIH_Knife::AIH_Knife()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(compBox);
	compBox->SetBoxExtent(FVector(50));
	compMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bow"));
	compMesh -> SetupAttachment(compBox);
	compMesh->SetRelativeLocation(FVector(-210.017793, -10.000000, 0.000002));
	compMesh->SetRelativeRotation(FRotator(0, 0, -90));
	compMesh->SetRelativeScale3D(FVector(1));
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/Bows/source/Bows_and_CrossBows_SketchFab_Ready_.Bows_and_CrossBows_SketchFab_Ready__Recurve Bow 2'"));
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
void AIH_Knife::BeginPlay()
{
	Super::BeginPlay();

	compBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Knife::OnOverlap);
	compBox->OnComponentEndOverlap.AddDynamic(this, &AIH_Knife::OnEndOverlap);
	compSphere->OnComponentBeginOverlap.AddDynamic(this, &AIH_Knife::OnSphereOverlap);
	compSphere->OnComponentEndOverlap.AddDynamic(this, &AIH_Knife::OnSphereEndOverlap);
}

// Called every frame
void AIH_Knife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIH_Knife::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);    // 오버랩된 플레이어를 캐스팅

	if (character != nullptr)    // 널이 아니라면 
	{
		character->pickBow = this;
// 		character->bgetbow = true;      //  활여부 = true, true일 때만 3번 키를 누를 수 있음.
// 		character->anim->isBow = true;  //   활 애님 = true
// 		character->ChangeToBow();   //  활 함수호출
// 
// 		character->anim->isGunEquipped = false;  // 총애님 no
// 
// 		Destroy(); 
	}
}

void AIH_Knife::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		character->pickBow = nullptr;
	}
}

void AIH_Knife::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(true);
	}
}

void AIH_Knife::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(false);
	}
}