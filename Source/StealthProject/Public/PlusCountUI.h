// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlusCountUI.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UPlusCountUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UCanvasPanel* PlusPanel;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* EnemyKill;

public:
	float delayTime;
	float removeTime = 2.0;
};
