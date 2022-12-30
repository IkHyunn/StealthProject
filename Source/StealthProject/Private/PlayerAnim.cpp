// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"     //  10. cast �ϱ� 
#include <GameFramework/CharacterMovementComponent.h>    // �÷��̾� �������� ���� �������� �����ؾ�
#include <GameFramework/Actor.h>




void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)    // [Event BLueprint Update Animation] ���� �������
{

//10. �÷��̾��� �̵��ӵ��� ������ SPEED�� �Ҵ��ϰ� �ʹ�
  
Super::NativeUpdateAnimation(DeltaSeconds);    // �θ� ����
auto ownerPawn = TryGetPawnOwner();      // 1.������ ������
auto player = Cast<ATPSPlayer>(ownerPawn);    //  2.�÷��̾�� ĳ�����ϱ�
if (player)    //������ ������ �������� ��
	{
	// ��� <-> �̵�
	FVector velocity = player->GetVelocity();
	FVector forwardVector = player->GetActorForwardVector();
	speed = FVector::DotProduct(forwardVector, velocity);   // speed �� �κ����� �������� �־� �־� 

	// �¿�ӵ� �Ҵ��ϱ�
	FVector rightVector = player->GetActorRightVector();
	direction = FVector::DotProduct(rightVector, velocity);  


	// ����
	auto movement = player->GetCharacterMovement();   // movement ������ ��Ƽ� ����
	isInAir = movement->IsFalling();    // �÷��̾ ���� ���߿� �ִ��� ���θ� ����ϰ� �ʹ�. �� ���� ��
	}
}
 // ���� �ִϸ��̼� ��Ÿ�� ��� �Լ� ����
void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
}