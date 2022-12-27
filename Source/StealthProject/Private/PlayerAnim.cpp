// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"     //  10. cast 하기 
#include <GameFramework/CharacterMovementComponent.h>    // 플레이어 공중존재 여부 변수에서 참조해야




void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)    // [Event BLueprint Update Animation] 노드와 같은기능
{

//10. 플레이어의 이동속도를 가져와 SPEED에 할당하고 싶다
  
Super::NativeUpdateAnimation(DeltaSeconds);    // 부모 실행
auto ownerPawn = TryGetPawnOwner();      // 1.소유폰 얻어오기
auto player = Cast<ATPSPlayer>(ownerPawn);    //  2.플레이어로 캐스팅하기
if (player)    //소유폰 얻어오기 성공햇을 때
	{
	// 대기 <-> 이동
	FVector velocity = player->GetVelocity();
	FVector forwardVector = player->GetActorForwardVector();
	speed = FVector::DotProduct(forwardVector, velocity);   // speed 에 두벡터의 내적값을 넣어 주어 

	// 점프
	auto movement = player->GetCharacterMovement();   // movement 변수에 담아서 쓰자
	isInAir = movement->IsFalling();    // 플레이어가 현재 공중에 있는지 여부를 기억하고 싶다. 참 거짓 값



	}




}