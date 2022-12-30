
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHPROJECT_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)    // 10-1  �÷��̾� �̵� �ӵ� 
	float speed = 0;      // 0�� �������� �ʴ� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)     // 20 �¿��̵��ӵ� ����
	float direction = 0;   


	virtual void NativeUpdateAnimation(float DeltaSeconds) override;   // 10. �������� ���ŵǴ� �Լ�

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;     // �÷��̾ ���߿� �ִ����� ����� ������ ����

	UPROPERTY(EditDefaultsOnly,Category = PlayerAnim)
	class UAnimMontage* attackAnimMontage;    // ����� ���� �ִϸ��̼� ��Ÿ��

	void PlayAttackAnim();    // ���� �ִϸ��̼� ��� �Լ�



};
