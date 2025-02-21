// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextAlarmUI.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UTextAlarmUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* InputKey;

	void InputKeyAnimation();
};
