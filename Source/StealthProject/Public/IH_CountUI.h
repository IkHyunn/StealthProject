// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IH_CountUI.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UIH_CountUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:

UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* currScoreUI;  // �ؽ�Ʈ �ڽ� ����

	void UpdateCurrScoreUI(int32 score); // ���� ������ ������Ʈ �� �Լ�

UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* completeText;
};
