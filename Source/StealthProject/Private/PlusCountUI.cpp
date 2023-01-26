// Fill out your copyright notice in the Description page of Project Settings.


#include "PlusCountUI.h"

void UPlusCountUI::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(EnemyKill, 0.0f, 1);
}

void UPlusCountUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	delayTime += InDeltaTime;

	if (delayTime > removeTime)
	{
		this->RemoveFromParent();

		delayTime = 0;
	}
}