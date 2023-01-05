// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"     //  10. cast 하기 
#include <GameFramework/CharacterMovementComponent.h>    // 플레이어 공중존재 여부 변수에서 참조해야
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
 // 어택 애니메이션 몽타주 재생 함수 구현
void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
}

// 암살 애니메이션 몽타주 재생 함수 구현
void UPlayerAnim::PlayAssasinateAnim()
{
	Montage_Play(assasinateAnimMontage);
}

// 주먹질 애니메이션 몽타주 
void UPlayerAnim::PlayPunchAnim()
{
	Montage_Play(punchAnimMontage);
}

// 
void UPlayerAnim::OnAttackAnimation()
{
	isPlayerAttack = true;  
}

void UPlayerAnim::EndAttackAnimation()
{
	isPlayerAttack = false;
}