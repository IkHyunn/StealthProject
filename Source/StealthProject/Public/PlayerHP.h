// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHP.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UPlayerHP : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* HPText;

public:
	void UpdatePlayerHP(float curr, float max);
};
