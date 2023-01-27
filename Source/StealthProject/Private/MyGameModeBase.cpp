// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "IH_MainMenu.h"
#include <Kismet/GameplayStatics.h>

AMyGameModeBase::AMyGameModeBase()
{
	ConstructorHelpers::FClassFinder<UIH_MainMenu> tempMainmenu(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_MainMenu.WG_MainMenu_C'"));
	if (tempMainmenu.Succeeded())
	{
		mainmenuWidget = tempMainmenu.Class;
	}
}

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	mainMenuUI = CreateWidget<UIH_MainMenu>(GetWorld(), mainmenuWidget);
}

void AMyGameModeBase::ShowMainMenuUI()
{
	mainMenuUI->AddToViewport();
	GetWorld(), GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}