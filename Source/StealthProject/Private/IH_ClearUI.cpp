// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_ClearUI.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Components/Button.h>

void UIH_ClearUI::NativeConstruct()
{

	btnRestart->OnClicked.AddDynamic(this, &UIH_ClearUI::Restart);  // btnRestart�� Ŭ���ϸ� Restart �Լ��� ����ȴ�.
	btnQuit->OnClicked.AddDynamic(this, &UIH_ClearUI::Quit);  // btnQuit�� Ŭ���ϸ� Quit �Լ��� ����ȴ�.
}

void UIH_ClearUI::Restart()
{
	UGameplayStatics::OpenLevel(GetWorld(), "StealthStage");
}

void UIH_ClearUI::Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}