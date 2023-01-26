// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Enemy.h"
#include "EnemyFSM.h"
#include <Components/BoxComponent.h>
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "EnemyAnim.h"
#include <Camera/CameraComponent.h>
#include <Components/SphereComponent.h>
#include <Components/WidgetComponent.h>
#include "EnemyHP.h"
#include "TextAlarmUI.h"

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
	compBox->SetBoxExtent(FVector(20, 25, 50));
	compBox->SetupAttachment(RootComponent);
	compBox->SetRelativeLocation(FVector(-60, 0, 0));

	// HandBox 컴포넌트 추가
	compHandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HandBox"));
	compHandBox->SetBoxExtent(FVector(10, 30, 10));
	compHandBox->SetupAttachment(GetMesh(),TEXT("Socket_RightHand"));
	compHandBox->SetRelativeLocation(FVector(0,-10,0));

	// 부드럽게 회전시키기
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Perception Scene Component 추가
	compEye = CreateDefaultSubobject<USceneComponent>(TEXT("Perception"));
	compEye->SetupAttachment(GetMesh(), TEXT("EyePerception"));

	// Perception Scene Component 추가
	compSpine = CreateDefaultSubobject<USceneComponent>(TEXT("PelvisPerception"));
	compSpine->SetupAttachment(GetMesh(), TEXT("SpinePerception"));

	assasinateCam = CreateDefaultSubobject<UCameraComponent>(TEXT("AssasinateCam"));
	assasinateCam->SetupAttachment(RootComponent);

	compSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	compSphere->SetupAttachment(RootComponent);
	compSphere->SetSphereRadius(250);

	compWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	compWidget->SetupAttachment(RootComponent);
	compWidget->SetRelativeLocation(FVector(0, 0, 100));

	ConstructorHelpers::FClassFinder<UEnemyHP>tempEnemyHP(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_EnemyHP.WG_EnemyHP_C'"));
	if (tempEnemyHP.Succeeded())
	{
		compWidget->SetWidgetClass(tempEnemyHP.Class);
		compWidget->SetWidgetSpace(EWidgetSpace::Screen);
		compWidget->SetVisibility(false);
	}

	compAlarmWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("AlarmWidget"));
	compAlarmWidget->SetupAttachment(RootComponent);
	compAlarmWidget->SetRelativeLocation(FVector(0, 0, 115));

	ConstructorHelpers::FClassFinder<UTextAlarmUI>tempAlarm(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_TextAlarmUI.WG_TextAlarmUI_C'"));
	if (tempAlarm.Succeeded())
	{
		compAlarmWidget->SetWidgetClass(tempAlarm.Class);
		compAlarmWidget->SetWidgetSpace(EWidgetSpace::Screen);
		compAlarmWidget->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void AIH_Enemy::BeginPlay()
{
	Super::BeginPlay();

	compBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Enemy::OnBackOverlap);
	compBox->OnComponentEndOverlap.AddDynamic(this, &AIH_Enemy::OnBackEndOverlap);

	compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	compHandBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_Enemy::OnHandOverlap);

	compSphere->OnComponentBeginOverlap.AddDynamic(this, &AIH_Enemy::OnSphereOverlap);
	compSphere->OnComponentEndOverlap.AddDynamic(this, &AIH_Enemy::OnSphereEndOverlap);

	enemyHPUI = Cast<UEnemyHP>(compWidget->GetUserWidgetObject());
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
	if (OtherActor != this)
	{
		character = Cast<ATPSPlayer>(OtherActor);

		if (character != nullptr)
		{
			character->isBack = true;
			compAlarmWidget->SetVisibility(true);
			//character->backEnemy = this;
		}
	}
}

void AIH_Enemy::OnBackEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != this)
	{
		character = Cast<ATPSPlayer>(OtherActor);

		if (character != nullptr)
		{
			character->isBack = false;
			compAlarmWidget->SetVisibility(false);
			//character->backEnemy = nullptr;
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
			if (character->HP > 0)
			{
				character->OnHitEvent();
			}
		}
	}
}

void AIH_Enemy::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		character = Cast<ATPSPlayer>(OtherActor);

		if (character != nullptr)
		{
			compWidget->SetVisibility(true);
		}
	}
}

void AIH_Enemy::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != this)
	{
		character = Cast<ATPSPlayer>(OtherActor);

		if (character != nullptr)
		{
			compWidget->SetVisibility(false);
		}
	}
}