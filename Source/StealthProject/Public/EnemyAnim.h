// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"
#include "EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=FSM)
	EEnemyState animState;		// 애니메이션 블루프린트 안에 EnemyFSM에서 만들었던 animState 열거형 변수를 만든다.

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	bool bAttackPlay = false;	// 애니메이션 블루프린트 안에 bAttackPlay 라는 Boolean 변수를 만든다.

	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* target;

	UFUNCTION(BlueprintCallable, Category=FSMEvent)
	void OnEndAttackAnimation();

// 	UFUNCTION(BlueprintCallable, Category=FSMEvent)
// 	void OnAttackAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
	void PlayDamageAnim(FName sectionName);  // 피격 애니메이션 재생 함수

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	bool bDieDone = false;  // 죽음상태 애니메이션 종료 여부
};
