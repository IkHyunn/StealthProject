// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IH_MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UIH_MainMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* btnGameStart;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* btnOption;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* btnGameQuit;

	UFUNCTION()
	void GameStart();

	UFUNCTION()
	void Option();

	UFUNCTION()
	void GameQuit();
};
