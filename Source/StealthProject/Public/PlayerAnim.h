
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
	UPlayerAnim();

protected:
	virtual void NativeBeginPlay() override;

public:

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;   // �������� ���ŵǴ� �Լ�

	UPROPERTY()
	class APawn* ownerPawn;

	UPROPERTY()
	class ATPSPlayer* player;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)    //�÷��̾� �̵��ӵ� 
	float speed = 0;      

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)     // �¿��̵��ӵ� 
	float direction = 0;   


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;     // ���߿� �ִ���

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isPunch = false;     // ��ġ �������

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isGunEquipped = false;  // �� �������

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = PlayerAnim)
	bool isBow = false;  // Ȱ �������

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = PlayerAnim)
	bool isKal = false;  // Į �������

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isCrouched = false;   //  ��ũ�Ǵ���

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isBowAiming = false;   //  Ȱ ���� ������

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* assasinateAnimMontage;  // �ϻ� ��Ÿ��
	void PlayAssasinateAnim();  // �ϻ� �ִϸ��̼� ��� �Լ�

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* punchAnimMontage;   // ��ġ ��Ÿ��
	void PlayPunchAnim();    // ��ġ �ִϸ��̼� ��� �Լ�

	UPROPERTY(EditAnywhere, Category = PlayerAnim)
	class UAnimMontage* hookAnimMontage;
	void PlayHookAnim();

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* BowAimAnimMontage;   // Ȱ���� ��Ÿ��
	void PlayBowAimAnim();    // Ȱ����  �ִϸ��̼� ��� �Լ� 

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* KalAnimMontage;   // Į���� ��Ÿ��
	void PlayKalAnim();    // Į����  �ִϸ��̼� ��� �Լ� 

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* Kal2AnimMontage;   // Į���� ��Ÿ��
	void PlayKal2Anim();

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* Kal3AnimMontage;   // Į���� ��Ÿ��
	void PlayKal3Anim();


	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* DamageDieMontage;   // �ǰ� ��Ÿ��

	//void PlayDamageDieAnim();    // �ǰ� (& ����) �ִϸ��̼� ��� �Լ�  ??

	//UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)   // �´� ��Ÿ��  ??
	//void PlayDamageAnim(FName sectionName);    // �ǰ� �ִϸ��̼� ��� �Լ�  ??

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isDieDone = false;  //  �������� �ִϸ��̼� ���Ῡ��

	UFUNCTION(BlueprintCallable, Category = PlayerAnim)
	void OnAttackAnimation();

	UFUNCTION(BlueprintCallable, Category = PlayerAnim)
	void EndAttackAnimation();

	UFUNCTION(BlueprintCallable, Category = PlayerAnim)
	void CallGameOver();

	UFUNCTION()
	void AnimNotify_AssasinateEnd();

	UFUNCTION()
	void AnimNotify_PlayerAttack();

	UFUNCTION()
	void AnimNotify_BowAttack();

	UFUNCTION()
	void AnimNotify_DamageEnd();

	UFUNCTION()
	void AnimNotify_PlayerAttackEnd();

	UFUNCTION()
	void AnimNotify_DieEnd();

	UFUNCTION()
	void AnimNotify_Crack();

	UFUNCTION()
	void AnimNotify_SaveAttack();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isPlayerAttack = false;

	UPROPERTY(EditDefaultsOnly) 
	TSubclassOf<class UCameraShakeBase> cameraShake;

};
