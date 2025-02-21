

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

	ownerPawn = TryGetPawnOwner();      // 소유폰 
	player = Cast<ATPSPlayer>(ownerPawn);    // 플레이어 캐스트
}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)    // [Event BLueprint Update Animation] 노드와 같은기능
{
	// 플레이어의 이동속도를 가져와 SPEED에 할당하고 싶다
	Super::NativeUpdateAnimation(DeltaSeconds);    // 부모 실행

	if (player)   
		{
		// 대기 <-> 이동
		FVector velocity = player->GetVelocity();
		FVector forwardVector = player->GetActorForwardVector();
		speed = FVector::DotProduct(forwardVector, velocity);   // 전후내적 speed

		// 좌우속도 할당하기
		FVector rightVector = player->GetActorRightVector();
		direction = FVector::DotProduct(rightVector, velocity);  // 좌우내적 derection


		// 점프
		auto movement = player->GetCharacterMovement();   // movement 
		isInAir = movement->IsFalling();    //  공중에 있는지 isInAir
		}
}

// 암살 애니메이션 몽타주 재생 함수 
void UPlayerAnim::PlayAssasinateAnim()
{
	Montage_Play(assasinateAnimMontage);
}

// 주먹질 애니메이션 몽타주 재생 함수 
void UPlayerAnim::PlayPunchAnim()
{
	Montage_Play(punchAnimMontage);
}

void UPlayerAnim::PlayHookAnim()
{
	Montage_Play(hookAnimMontage);
}

// 활시위 애니메이션 몽타주 재생 함수 
void UPlayerAnim::PlayBowAimAnim()
{
	Montage_Play(BowAimAnimMontage);
}

// 칼공격 애니메이션 몽타주 재생 함수 
void UPlayerAnim::PlayKalAnim()
{
	Montage_Play(KalAnimMontage);
}

void UPlayerAnim::PlayKal2Anim()
{
	Montage_Play(Kal2AnimMontage);
}

void UPlayerAnim::PlayKal3Anim()
{
	Montage_Play(Kal3AnimMontage);
}

// 데미지 받고 죽는 함수  ???
//void UPlayerAnim::PlayDamageDieAnim()
//{
//	Montage_Play(DamageDieMontage); 
//}

// 피격 애니메이션 몽타주 재생 함수    ????
// void UPlayerAnim::PlayDamageAnim(FName sectionName)
// {
// 	Montage_Play(DamageMontage);
// }

void UPlayerAnim::AnimNotify_PlayerAttack()
{
	player->righthandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	player->lefthandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	player->knifeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void UPlayerAnim::AnimNotify_SaveAttack()
{
	player->ComboAttackSave();
}

void UPlayerAnim::AnimNotify_BowAttack()
{
	player->LineTrace();
}

// AttackEnd 애님 노티파이
void UPlayerAnim::AnimNotify_PlayerAttackEnd()
{
	player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	player->ComboReset();
}

void UPlayerAnim::AnimNotify_Crack()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	playerController->PlayerCameraManager->StartCameraShake(cameraShake);
}

void UPlayerAnim::AnimNotify_AssasinateEnd()
{
	player->bUseControllerRotationYaw=true;
	player->SetActorLocation(player->GetActorLocation()+player->GetActorForwardVector()*175);

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	player->EnableInput(playerController);
	playerController->SetViewTargetWithBlend(player, 0.5);
}

// DamageEnd 애님 노티파이
void UPlayerAnim::AnimNotify_DamageEnd()
{
	player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

// DieEnd 애님 노티파이
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