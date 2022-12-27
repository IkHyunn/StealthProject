
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

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;   // 10. 매프레임 갱신되는 함수

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;     // 플레이어가 공중에 있는지를 기억할 변수를 선언
};
