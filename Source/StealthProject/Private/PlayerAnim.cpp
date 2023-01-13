

#include "PlayerAnim.h"
#include "TPSPlayer.h"     
#include <GameFramework/CharacterMovementComponent.h>    
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

// �´� �ִϸ��̼� ��Ÿ�� ��� �Լ� 
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