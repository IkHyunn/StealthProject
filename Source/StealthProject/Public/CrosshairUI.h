// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairUI.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UCrosshairUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;


// public:
// 	UPROPERTY(meta = (BindWidget))
// 		class UImage* CrossHair_Right;
// 
// 	UPROPERTY(meta = (BindWidget))
// 		class UImage* CrossHair_Left;
// 
// 	UPROPERTY(meta = (BindWidget))
// 		class UImage* CrossHair_Up;
// 
// 	UPROPERTY(meta = (BindWidget))
// 		class UImage* CrossHair_Down;

public:
	void ZoomIn();
	void ZoomOut();

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ZoomAnimation;
};
