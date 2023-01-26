// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHP.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UEnemyHP : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* enemyHP;

public:
	void UpdateEnemyHP(float curr, float max);
};
