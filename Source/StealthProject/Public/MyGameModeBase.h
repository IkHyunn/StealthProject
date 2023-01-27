// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameModeBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UIH_MainMenu> mainmenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UIH_MainMenu* mainMenuUI;

public:
	UFUNCTION(BlueprintCallable)
		void ShowMainMenuUI();
};
