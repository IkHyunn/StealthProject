// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowCountUI.h"
#include <Components/TextBlock.h>

void UArrowCountUI::UpdateCurrentArrow(int32 arrow)
{
	leftArrow->SetText(FText::AsNumber(arrow));
}