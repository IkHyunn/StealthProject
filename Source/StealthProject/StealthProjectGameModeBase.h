// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StealthProjectGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API AStealthProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStealthProjectGameModeBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	int32 currScore = 0;  // 현재점수

public:
	void AddScore(int32 addValue);  // 점수를 증가시키는 함수, 점수가 올라갈 때 호출

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UIH_CountUI> countWidget;  // UIH_CountUI 클래스를 가져와 countWidget에 저장해둔다.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UIH_CountUI* countUI;  // UIH_CountUI 클래스를 countUI에 저장한다.

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UIH_GameOverUI> gameoverWidget;  //UIH_GameOverUI 클래스를 가져와 gameoverWidget에 저장해둔다.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UIH_GameOverUI* gameoverUI;

	void ShowGameOverUI();  // GameOverUI를 띄우는 함수, 캐릭터가 죽었을 때 호출
};