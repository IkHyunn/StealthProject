// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Enemy.h"
#include "EnemyFSM.h"
#include <Components/BoxComponent.h>
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>
#include "TPSPlayer.h"

// Sets default values
AIH_Enemy::AIH_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Wise/Character/Enemy/Meshes/Ch15_nonPBR.Ch15_nonPBR'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	}

	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("AnimBlueprint'/Game/Wise/Animations/Enemy/ABP_IH_Enemy.ABP_IH_Enemy_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BehindBox"));
	compBox->SetBoxExtent(FVector(30, 40, 50));
	compBox->SetupAttachment(RootComponent);
	compBox->SetRelativeLocation(FVector(-60, 0, 0));

	compBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Enemy::OnOverlap);
}

// Called when the game starts or when spawned
void AIH_Enemy::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AIH_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AIH_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AIH_Enemy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Player::StaticClass());
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		character->isBack = true;
	}
}