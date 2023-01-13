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
	
protected:
	virtual void NativeBeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=FSM)
	EEnemyState animState;		// �ִϸ��̼� �������Ʈ �ȿ� EnemyFSM���� ������� animState ������ ������ �����.

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	bool bAttackPlay = false;	// �ִϸ��̼� �������Ʈ �ȿ� bAttackPlay ��� Boolean ������ �����.

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	bool isOnHit = false;

	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* target;

	UFUNCTION(BlueprintCallable, Category=FSMEvent)
	void OnEndAttackAnimation();

	UFUNCTION(BlueprintCallable, Category=FSMEvent)
	void OnAttackAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
	void PlayDamageAnim(FName sectionName);  // �ǰ� �ִϸ��̼� ��� �Լ�

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	bool bDieDone = false;  // �������� �ִϸ��̼� ���� ����

	UFUNCTION()
	void AnimNotify_HangingStart();
};
