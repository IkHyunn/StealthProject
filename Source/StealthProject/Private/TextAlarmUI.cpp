// Fill out your copyright notice in the Description page of Project Settings.


#include "TextAlarmUI.h"
#include <Components/TextBlock.h>

void UTextAlarmUI::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(InputKey, 0.0f, 0);
}