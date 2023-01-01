// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include <Kismet/GameplayStatics.h>
#include "TPSPlayer.h"


void UEnemyAnim::OnAttackAnimation()
{
	isOnHit = true;
}

void UEnemyAnim::OnEndAttackAnimation()
{
	isOnHit = false;
}

// void UEnemyAnim::OnAttackAnimation()
// {
// 	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
// 	target = Cast<ATPSPlayer>(actor);
// 
// 	target->OnHitEvent();
// }