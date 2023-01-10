
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

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;   // 매프레임 갱신되는 함수

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)    //플레이어 이동속도 
	float speed = 0;      

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)     // 좌우이동속도 
	float direction = 0;   


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;     // 공중에 있는지

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isGunEquipped = false;  // 총 들었는지

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = PlayerAnim)
	bool isBow = false;  // 활 들었는지

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isCrouched = false;   //  웅크렷는지

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* attackAnimMontage;    // 공격 몽타주

	void PlayAttackAnim();    // 공격 애니메이션 재생 함수

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* assasinateAnimMontage;  // 암살 몽타주

	void PlayAssasinateAnim();  // 암살

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	class UAnimMontage* punchAnimMontage;   // 주먹질 몽타주

	void PlayPunchAnim();    // 주먹질

	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* BowAimAnimMontage;   // 활시위 몽타주

	void PlayBowAimAnim();    // 활시위   

	UFUNCTION(BlueprintCallable, Category = PlayerAnim)
	void OnAttackAnimation();

	UFUNCTION(BlueprintCallable, Category = PlayerAnim)
	void EndAttackAnimation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isPlayerAttack = false;

};
