// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include <Kismet/GameplayStatics.h>
#include "TPSPlayer.h"
#include "IH_Enemy.h"

void UEnemyAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UEnemyAnim::OnAttackAnimation()
{
	isOnHit = true;
}

void UEnemyAnim::OnEndAttackAnimation()
{
	isOnHit = false;
}

void UEnemyAnim::AnimNotify_HangingStart()
{
	APawn* pawn = TryGetPawnOwner();
	AIH_Enemy* enemy = Cast<AIH_Enemy>(pawn);

	enemy->fsm->isLooking = true;
}

// void UEnemyAnim::OnAttackAnimation()
// {
// 	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
// 	target = Cast<ATPSPlayer>(actor);
// 
// 	target->OnHitEvent();
// }