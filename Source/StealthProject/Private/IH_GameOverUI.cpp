// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_GameOverUI.h"
#include <Kismet/GameplayStatics.h>
#include <Components/Button.h>
#include <Sound/SoundBase.h>
#include <Blueprint/UserWidget.h>

void UIH_GameOverUI::NativeConstruct()
{
	PlayAnimation(GameOver, 0.0f, 1);
	btnRetry->OnClicked.AddDynamic(this, &UIH_GameOverUI::Retry);
	btnQuit->OnClicked.AddDynamic(this, &UIH_GameOverUI::Quit);
}

void UIH_GameOverUI::Retry()
{
	UGameplayStatics::OpenLevel(GetWorld(), "StealthStage");
}

void UIH_GameOverUI::Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}