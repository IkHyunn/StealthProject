// Copyright Epic Games, Inc. All Rights Reserved.


#include "StealthProjectGameModeBase.h"
#include "IH_CountUI.h"
#include "IH_GameOverUI.h"

void AStealthProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	countUI = CreateWidget<UIH_CountUI>(GetWorld(), countWidget);
	countUI->AddToViewport();
	
}

void AStealthProjectGameModeBase::AddScore(int32 addValue)
{
	currScore += addValue;
	
	UE_LOG(LogTemp, Warning, TEXT("currScore : %d"), currScore);

	countUI->UpdateCurrScoreUI(currScore);
}

void AStealthProjectGameModeBase::ShowGameOverUI()
{
	gameoverUI = CreateWidget<UIH_GameOverUI>(GetWorld(), gameoverWidget);
	gameoverUI->AddToViewport();
}