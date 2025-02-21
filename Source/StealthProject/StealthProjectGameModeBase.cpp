// Copyright Epic Games, Inc. All Rights Reserved.


#include "StealthProjectGameModeBase.h"
#include "IH_CountUI.h"
#include "IH_GameOverUI.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Components/TextBlock.h>

AStealthProjectGameModeBase::AStealthProjectGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FClassFinder<UIH_CountUI> tempCount(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_CountUI.WG_CountUI_C'"));
	if (tempCount.Succeeded())
	{
		countWidget = tempCount.Class;
	}

	ConstructorHelpers::FClassFinder<UIH_GameOverUI> tempGameover(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_GameOverUI.WG_GameOverUI_C'"));
	if (tempGameover.Succeeded())
	{
		gameoverWidget = tempGameover.Class;
	}
}

void AStealthProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	countUI = CreateWidget<UIH_CountUI>(GetWorld(), countWidget);
}

void AStealthProjectGameModeBase::AddScore(int32 addValue)
{
	currScore += addValue;

	countUI->UpdateCurrScoreUI(currScore);

	if (currScore >= 5)
	{
		countUI->completeText->SetVisibility(ESlateVisibility::Visible);
	}
}

void AStealthProjectGameModeBase::ShowGameOverUI()
{
	gameoverUI = CreateWidget<UIH_GameOverUI>(GetWorld(), gameoverWidget);
	gameoverUI->AddToViewport();
//	UGameplayStatics::SetGamePaused(GetWorld(), true);  // ������ �����.
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);  // ���콺 Ŀ���� ȭ�鿡 ����.
}