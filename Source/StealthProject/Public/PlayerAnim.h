
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

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;   // 매프레임 갱신되는 함수

	UPROPERTY()
	class APawn* ownerPawn;

	UPROPERTY()
	class ATPSPlayer* player;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)    //플레이어 이동속도 
	float speed = 0;      

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)     // 좌우이동속도 
	float direction = 0;   


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;     // 공중에 있는지

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isPunch = false;     // 펀치 들었는지

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isGunEquipped = false;  // 총 들었는지

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = PlayerAnim)
	bool isBow = false;  // 활 들었는지

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = PlayerAnim)
	bool isKal = false;  // 칼 들었는지

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isCrouched = false;   //  웅크렷는지

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* attackAnimMontage;    // 총 몽타주
	void PlayAttackAnim();    // 총 애니메이션 재생 함수

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* assasinateAnimMontage;  // 암살 몽타주
	void PlayAssasinateAnim();  // 암살 애니메이션 재생 함수

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* punchAnimMontage;   // 펀치 몽타주
	void PlayPunchAnim();    // 펀치 애니메이션 재생 함수

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* BowAimAnimMontage;   // 활시위 몽타주
	void PlayBowAimAnim();    // 활시위  애니메이션 재생 함수 

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* KalAimAnimMontage;   // 칼공격 몽타주
	void PlayKalAimAnim();    // 칼공격  애니메이션 재생 함수 


	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* DamageDieMontage;   // 피격 몽타주

	//void PlayDamageDieAnim();    // 피격 (& 죽음) 애니메이션 재생 함수  ??

	//UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)   // 맞는 몽타주  ??
	//void PlayDamageAnim(FName sectionName);    // 피격 애니메이션 재생 함수  ??

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isDieDone = false;  //  죽음상태 애니메이션 종료여부

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
	void AnimNotify_DamageEnd();

	UFUNCTION()
	void AnimNotify_PlayerAttackEnd();

	UFUNCTION()
	void AnimNotify_DieEnd();

	UFUNCTION()
	void AnimNotify_Crack();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isPlayerAttack = false;

	UPROPERTY(EditDefaultsOnly) 
	TSubclassOf<class UCameraShakeBase> cameraShake;

};
