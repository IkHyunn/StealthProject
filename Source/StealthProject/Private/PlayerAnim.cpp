// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"     //  10. cast �ϱ� 
#include <GameFramework/CharacterMovementComponent.h>    // �÷��̾� �������� ���� �������� �����ؾ�
#include <GameFramework/Actor.h>




void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)    // [Event BLueprint Update Animation] ���� �������
{
// �÷��̾��� �̵��ӵ��� ������ SPEED�� �Ҵ��ϰ� �ʹ�
Super::NativeUpdateAnimation(DeltaSeconds);    // �θ� ����

auto ownerPawn = TryGetPawnOwner();      // ������ 
auto player = Cast<ATPSPlayer>(ownerPawn);    // �÷��̾� ĳ��Ʈ

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
 // ���� �ִϸ��̼� ��Ÿ�� ��� �Լ� ����
void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
}

// �ϻ� �ִϸ��̼� ��Ÿ�� ��� �Լ� ����
void UPlayerAnim::PlayAssasinateAnim()
{
	Montage_Play(assasinateAnimMontage);
}

// �ָ��� �ִϸ��̼� ��Ÿ�� 
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