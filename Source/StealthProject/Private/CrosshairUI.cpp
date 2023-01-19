// Fill out your copyright notice in the Description page of Project Settings.


#include "CrosshairUI.h"

void UCrosshairUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCrosshairUI::ZoomIn()
{
	PlayAnimation(ZoomAnimation, 0.0f, 1);
}

void UCrosshairUI::ZoomOut()
{
	PlayAnimationReverse(ZoomAnimation);
}