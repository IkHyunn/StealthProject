// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHP.h"
#include <Components/TextBlock.h>
#include <Components/ProgressBar.h>

void UPlayerHP::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerHP::UpdatePlayerHP(float curr, float max)
{
	float hp = curr/max;

	HPText->SetText(FText::AsNumber(hp*100));
	HPBar->SetPercent(hp);
}