

#include "PlayerAnim.h"
#include "TPSPlayer.h"     
#include <GameFramework/CharacterMovementComponent.h>    
#include <GameFramework/Actor.h>
#include "../StealthProjectGameModeBase.h"
#include <GameFramework/Pawn.h>
#include <Components/BoxComponent.h>
#include <GameFramework/PlayerController.h>

UPlayerAnim::UPlayerAnim()
{
	ConstructorHelpers::FClassFinder<UCameraShakeBase> tempShake(TEXT("Blueprint'/Game/Wise/Blueprints/BP_CamerShake.BP_CamerShake_C'"));
	if (tempShake.Succeeded())
	{
		cameraShake = tempShake.Class;
	}
}

void UPlayerAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ownerPawn = TryGetPawnOwner();      // ������ 
	player = Cast<ATPSPlayer>(ownerPawn);    // �÷��̾� ĳ��Ʈ
}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)    // [Event BLueprint Update Animation] ���� �������
{
// �÷��̾��� �̵��ӵ��� ������ SPEED�� �Ҵ��ϰ� �ʹ�
Super::NativeUpdateAnimation(DeltaSeconds);    // �θ� ����

if (player)   
	{
	// ��� <-> �̵�
	FVector velocity = player->GetVelocity();
	FVector forwardVector = player->GetActorForwardVector();
	speed = FVector::DotProduct(forwardVector, velocity);   // ���ĳ��� speed

	// �¿�ӵ� �Ҵ��ϱ�
	FVector rightVector = player->GetActorRightVector();
	direction = FVector::DotProduct(rightVector, velocity);  // �¿쳻�� derection


	// ����
	auto movement = player->GetCharacterMovement();   // movement 
	isInAir = movement->IsFalling();    //  ���߿� �ִ��� isInAir
	}
}
 // �� ��� �ִϸ��̼� ��Ÿ�� ��� �Լ� 
void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
}

// �ϻ� �ִϸ��̼� ��Ÿ�� ��� �Լ� 
void UPlayerAnim::PlayAssasinateAnim()
{
	Montage_Play(assasinateAnimMontage);
}

// �ָ��� �ִϸ��̼� ��Ÿ�� ��� �Լ� 
void UPlayerAnim::PlayPunchAnim()
{
	Montage_Play(punchAnimMontage);
}

// Ȱ���� �ִϸ��̼� ��Ÿ�� ��� �Լ� 
void UPlayerAnim::PlayBowAimAnim()
{
	Montage_Play(BowAimAnimMontage);
}

// Į���� �ִϸ��̼� ��Ÿ�� ��� �Լ� 
void UPlayerAnim::PlayKalAimAnim()
{
	Montage_Play(KalAimAnimMontage);
}

// ������ �ް� �״� �Լ�  ???
//void UPlayerAnim::PlayDamageDieAnim()
//{
//	Montage_Play(DamageDieMontage); 
//}

// �ǰ� �ִϸ��̼� ��Ÿ�� ��� �Լ�    ????
// void UPlayerAnim::PlayDamageAnim(FName sectionName)
// {
// 	Montage_Play(DamageMontage);
// }

void UPlayerAnim::AnimNotify_PlayerAttack()
{
	//player->compBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

// AttackEnd �ִ� ��Ƽ����
void UPlayerAnim::AnimNotify_PlayerAttackEnd()
{
	player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	player->compBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void UPlayerAnim::AnimNotify_Crack()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	playerController->PlayerCameraManager->StartCameraShake(cameraShake);

	playerController->SetViewTargetWithBlend(player, 0.5);
}

void UPlayerAnim::AnimNotify_AssasinateEnd()
{
	player->SetActorLocation(player->GetActorLocation()+player->GetActorForwardVector()*180);
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	player->EnableInput(playerController);
}

// DamageEnd �ִ� ��Ƽ����
void UPlayerAnim::AnimNotify_DamageEnd()
{
	player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

// DieEnd �ִ� ��Ƽ����
void UPlayerAnim::AnimNotify_DieEnd()
{
	CallGameOver();
}
 
void UPlayerAnim::OnAttackAnimation()
{
	isPlayerAttack = true;  
}
//
void UPlayerAnim::EndAttackAnimation()
{
	isPlayerAttack = false;
}
void UPlayerAnim::CallGameOver()
{
	AStealthProjectGameModeBase* gameMode = GetWorld()->GetAuthGameMode< AStealthProjectGameModeBase>();

	gameMode->ShowGameOverUI();
}