// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_CountUI.h"
#include <Components/TextBlock.h>

void UIH_CountUI::NativeConstruct()
{
	Super::NativeConstruct();

	completeText->SetVisibility(ESlateVisibility::Hidden);
}

void UIH_CountUI::UpdateCurrScoreUI(int32 score)
{
	currScoreUI->SetText(FText::AsNumber(score));  // currScoreUI�� Text�� �Ű������� ������ score������ �Ѵ�.
}