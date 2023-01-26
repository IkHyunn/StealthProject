// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_Pointer.h"

void UIH_Pointer::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(PickUp, 0.0f, 0);
}