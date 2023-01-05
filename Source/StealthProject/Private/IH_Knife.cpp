// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Knife.h"
#include <Components/BoxComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "TPSPlayer.h"
#include "PlayerAnim.h"

// Sets default values
AIH_Knife::AIH_Knife()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(compBox);
	compBox->SetBoxExtent(FVector(50));

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
	character = Cast<ATPSPlayer>(OtherActor);    // 오버랩된 플레이어를 캐스팅

	if (character != nullptr)    // 널이 아니라면 
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup Knife"));
		character->bgetKnife = true;      //  칼여부 = true, true일 때만 3번 키를 누를 수 있음.
		character->anim->isKnife = true;  //   칼애님 실행
		character->anim->isGunEquipped = false;  // 총애님 = false
		character->ChangeToKnife();   //  칼함수호출
		Destroy(); 
	}
}