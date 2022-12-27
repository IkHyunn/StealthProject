// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IH_ClearUI.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UIH_ClearUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* btnRestart;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* btnQuit;

	UFUNCTION()
	void Restart();

	UFUNCTION()
	void Quit();
	
};
