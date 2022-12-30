// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_MainMenu.h"
#include <Components/Button.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>

void UIH_MainMenu::NativeConstruct()
{
	btnGameStart->OnClicked.AddDynamic(this, &UIH_MainMenu::GameStart);
	btnGameQuit->OnClicked.AddDynamic(this, &UIH_MainMenu::GameQuit);
	btnOption->OnClicked.AddDynamic(this, &UIH_MainMenu::Option);
}

void UIH_MainMenu::GameStart()
{
	UGameplayStatics::OpenLevel(GetWorld(),"StealthStage");
}

void UIH_MainMenu::GameQuit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(),GetWorld()->GetFirstPlayerController(), EQuitPreference::Background, false);
}

void UIH_MainMenu::Option()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Option Open"));
}