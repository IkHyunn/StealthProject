// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IH_GameOverUI.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UIH_GameOverUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* btnRetry;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* btnQuit;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* GameOver;

	UFUNCTION()
	void Retry();  // �ٽ��ϱ� ��ư �Լ�
	UFUNCTION()
	void Quit();  // �����ϱ� ��ư �Լ�

};
