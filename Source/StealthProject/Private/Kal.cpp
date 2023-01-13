// Fill out your copyright notice in the Description page of Project Settings.


#include "Kal.h"
#include "TPSPlayer.h"
#include <Components/BoxComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "PlayerAnim.h"

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
	compMesh->SetRelativeLocation(FVector(0, -10, 0));
	compMesh->SetRelativeScale3D(FVector(1.5));
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Knife_A.Knife_A'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetSkeletalMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void AKal::BeginPlay()
{
	Super::BeginPlay();

	compBox->OnComponentBeginOverlap.AddDynamic(this, &AKal::OnOverlap);
	
}

// Called every frame
void AKal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKal::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);    // �������� �÷��̾ ĳ����

	if (character != nullptr)    // ���� �ƴ϶�� 
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup Kal"));
		character->bgetKal = true;      //  Į���� = true, true�� ���� 3�� Ű�� ���� �� ����.
		character->anim->isBow = true;  //   Į �ִ� = true
		character->ChangeToKal();   //  Į �Լ�ȣ��
		character->anim->isGunEquipped = false;  // �Ѿִ� no

		Destroy();
	}
}