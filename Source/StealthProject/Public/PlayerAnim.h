
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)    // 10-1  플레이어 이동 속도 
	float speed = 0;      // 0은 움직이지 않는 상태

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)     // 20 좌우이동속도 변수
	float direction = 0;   


	virtual void NativeUpdateAnimation(float DeltaSeconds) override;   // 10. 매프레임 갱신되는 함수

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;     // 플레이어가 공중에 있는지를 기억할 변수를 선언

	UPROPERTY(EditDefaultsOnly,Category = PlayerAnim)
	class UAnimMontage* attackAnimMontage;    // 재생할 공격 애니메이션 몽타주

	void PlayAttackAnim();    // 공격 애니메이션 재생 함수



};
