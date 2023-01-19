// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Player.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "Components/CapsuleComponent.h"
#include "EnemyFSM.h"
#include "IH_Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "../StealthProjectGameModeBase.h"

// Sets default values
AIH_Player::AIH_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;

	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CamearaComponent"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void AIH_Player::BeginPlay()
{
	Super::BeginPlay();
	
	HP = initialHP;
}

// Called every frame
void AIH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();

	currentTime += DeltaTime;
}

// Called to bind functionality to input
void AIH_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Turn Right / Left Mouse"), this, &AIH_Player::Turn);
	PlayerInputComponent->BindAxis(TEXT("Look Up / Down Mouse"), this, &AIH_Player::LookUp);

	PlayerInputComponent->BindAxis(TEXT("Move Forward / Backward"), this, &AIH_Player::InputVertical);
	PlayerInputComponent->BindAxis(TEXT("Move Right / Left"), this, &AIH_Player::InputHorizontal);
	
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AIH_Player::InputJump);
//	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &AIH_Player::InputAttack);

/*	PlayerInputComponent->BindAction(TEXT("Assasinate"), IE_Released, this, &AIH_Player::InputAssasinate);*/
}

void AIH_Player::Move()
{
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	/*FVector P = GetActorLocation() + direction * walkSpeed * DeltaTime;		// 위치 = 현재위치 + 방향 * 속도 * 시간
	SetActorLocation(P);*/
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void AIH_Player::Turn(float value)
{
	AddControllerYawInput(value);
}

void AIH_Player::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void AIH_Player::InputVertical(float value)
{
	direction.X = value;
}

void AIH_Player::InputHorizontal(float value)
{
	direction.Y = value;
}

void AIH_Player::InputJump()
{
	Jump();
}

// void AIH_Player::InputAttack()
// {
// 	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Enemy::StaticClass());  // AIH_Enemy를 찾아서 actor 변수에 넣어둔다.
// 	AIH_Enemy* enemy = Cast<AIH_Enemy>(actor);  // actor가 AIH_Enemy가 맞으면 enemy 변수에 저장한다.
// 	UEnemyFSM* enemyFSM = Cast<UEnemyFSM>(enemy);
// 
// 	if (currentTime > attackDelayTime)
// 	{
// 		enemy->fsm->OnDamageProcess();
// 		currentTime = 0;
// 	}
// }

// void AIH_Player::InputAssasinate()
// {
// 	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Enemy::StaticClass());
// 	AIH_Enemy* enemy = Cast<AIH_Enemy>(actor);
// 	UEnemyFSM* enemyFSM = Cast<UEnemyFSM>(enemy);
// 
// 	if (isBack == true)
// 	{
// 		enemy->fsm->OnBackAttack();
// 		isBack = false;
// 	}
// }
// 
// void AIH_Player::OnHitEvent()
// {
// 	UE_LOG(LogTemp, Warning, TEXT("Player Damaged!"));
// 	HP--;
// 	UE_LOG(LogTemp, Warning, TEXT("Player HP : %d"), HP);
// 
// 	if (HP <= 0)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Player Dead!"));
// 		OnGameOver();
// 	}
// }

// void AIH_Player::OnGameOver()
// {
// 	UGameplayStatics::SetGamePaused(GetWorld(), true);
// 	AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();
// 
// 	currMode->ShowGameOverUI();
// 	UGameplayStatics::SetGamePaused(GetWorld(), true);
// 	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
// }