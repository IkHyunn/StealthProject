// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHP.h"
#include <Components/ProgressBar.h>

void UEnemyHP::UpdateEnemyHP(float curr, float max)
{
	float hp = curr/max;
	
	enemyHP -> SetPercent(hp);
}