// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include <Kismet/GameplayStatics.h>
#include "TPSPlayer.h"
#include "IH_Enemy.h"
#include <GameFramework/Pawn.h>
#include <Components/BoxComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

void UEnemyAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ownerPawn = TryGetPawnOwner();
	me = Cast<AIH_Enemy>(ownerPawn);

	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	target = Cast<ATPSPlayer>(actor);
}

void UEnemyAnim::OnAttackAnimation()
{
	isOnHit = true;
}

void UEnemyAnim::OnEndAttackAnimation()
{
	isOnHit = false;
}

void UEnemyAnim::AnimNotify_DieEnd()
{
	bDieDone = true;
	target->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void UEnemyAnim::AnimNotify_OnAttack()
{
	me->compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UEnemyAnim::AnimNotify_AttackEnd()
{
	me->compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void UEnemyAnim::AnimNotify_HangingStart()
{
	me->fsm->isLooking = true;
}

// void UEnemyAnim::OnAttackAnimation()
// {
// 	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
// 	target = Cast<ATPSPlayer>(actor);
// 
// 	target->OnHitEvent();
// }