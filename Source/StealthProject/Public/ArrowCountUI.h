// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArrowCountUI.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UArrowCountUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* leftArrow;

	UFUNCTION()
	void UpdateCurrentArrow(int32 arrow);
};
