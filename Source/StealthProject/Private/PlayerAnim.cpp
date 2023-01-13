

#include "PlayerAnim.h"
#include "TPSPlayer.h"     
#include <GameFramework/CharacterMovementComponent.h>    
#include <GameFramework/Actor.h>




void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)    // [Event BLueprint Update Animation] 노드와 같은기능
{
// 플레이어의 이동속도를 가져와 SPEED에 할당하고 싶다
Super::NativeUpdateAnimation(DeltaSeconds);    // 부모 실행

auto ownerPawn = TryGetPawnOwner();      // 소유폰 
auto player = Cast<ATPSPlayer>(ownerPawn);    // 플레이어 캐스트

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
 // 총 어깨 애니메이션 몽타주 재생 함수 
void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
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

// 활시위 애니메이션 몽타주 재생 함수 
void UPlayerAnim::PlayBowAimAnim()
{
	Montage_Play(BowAimAnimMontage);
}

// 맞는 애니메이션 몽타주 재생 함수 
//void UPlayerAnim::PlayDamageAnim()
//{
//	Montage_Play(DamageMontage);
//}
// 
void UPlayerAnim::OnAttackAnimation()
{
	isPlayerAttack = true;  
}
//
void UPlayerAnim::EndAttackAnimation()
{
	isPlayerAttack = false;
}