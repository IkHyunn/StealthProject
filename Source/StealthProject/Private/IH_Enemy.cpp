// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Enemy.h"
#include "EnemyFSM.h"
#include <Components/BoxComponent.h>
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "EnemyAnim.h"

// Sets default values
AIH_Enemy::AIH_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스켈레탈 메시 추가
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Wise/Character/Enemy/Meshes/Ch15_nonPBR.Ch15_nonPBR'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	}

	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	// 애니메이션 블루프린트 추가
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("AnimBlueprint'/Game/Wise/Animations/Enemy/ABP_IH_Enemy.ABP_IH_Enemy_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	// BehindBox 컴포넌트 추가
	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BehindBox"));
	compBox->SetBoxExtent(FVector(20, 40, 50));
	compBox->SetupAttachment(RootComponent);
	compBox->SetRelativeLocation(FVector(-70, 0, 0));


	// HandBox 컴포넌트 추가
	compHandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HandBox"));
	compHandBox->SetBoxExtent(FVector(20));
	compHandBox->SetupAttachment(GetMesh(),TEXT("Socket_RightHand"));
	compHandBox->SetRelativeLocation(FVector(0,0,0));


	// 부드럽게 회전시키기
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AIH_Enemy::BeginPlay()
{
	Super::BeginPlay();
	compBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Enemy::OnBackOverlap);
	compHandBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Enemy::OnHandOverlap);
//	compHandBox->OnComponentEndOverlap.AddDynamic(this, &AIH_Enemy::OnHandEndOverlap);

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

void AIH_Enemy::OnBackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Player::StaticClass());
	if (OtherActor != this)
	{
		character = Cast<ATPSPlayer>(OtherActor);

		if (character != nullptr)
		{
			character->isBack = true;
		}
	}
}

void AIH_Enemy::OnHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		character = Cast<ATPSPlayer>(OtherActor);
		
		if (character != nullptr)
		{
			if (fsm->anim->isOnHit == true)
			{
			character->OnHitEvent();
			}
		}
	}
}

//void AIH_Enemy::OnHandEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
// 	if (OtherActor != this)
// 	{
// 		character = Cast<ATPSPlayer>(OtherActor);
// 
// 		if (character != nullptr)
// 		{
// 	
// 		}
// 	}
//}