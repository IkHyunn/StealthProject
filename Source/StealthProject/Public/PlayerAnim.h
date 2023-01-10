
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

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;   // �������� ���ŵǴ� �Լ�

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)    //�÷��̾� �̵��ӵ� 
	float speed = 0;      

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)     // �¿��̵��ӵ� 
	float direction = 0;   


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;     // ���߿� �ִ���

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isGunEquipped = false;  // �� �������

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = PlayerAnim)
	bool isBow = false;  // Ȱ �������

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isCrouched = false;   //  ��ũ�Ǵ���

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* attackAnimMontage;    // ���� ��Ÿ��

	void PlayAttackAnim();    // ���� �ִϸ��̼� ��� �Լ�

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* assasinateAnimMontage;  // �ϻ� ��Ÿ��

	void PlayAssasinateAnim();  // �ϻ�

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* punchAnimMontage;   // �ָ��� ��Ÿ��

	void PlayPunchAnim();    // �ָ���

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* BowAimAnimMontage;   // Ȱ���� ��Ÿ��

	void PlayBowAimAnim();    // Ȱ����   

	UFUNCTION(BlueprintCallable, Category = PlayerAnim)
	void OnAttackAnimation();

	UFUNCTION(BlueprintCallable, Category = PlayerAnim)
	void EndAttackAnimation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isPlayerAttack = false;

};
