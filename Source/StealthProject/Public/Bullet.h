
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class STEALTHPROJECT_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


// 변수 선언 --------------------------------------------
	//                                        float ddd = 1000;    속력변수  -> 무브먼트로 필요 없음




	// 컴포넌트 선언 
	
	// 콜리젼
	UPROPERTY(VisibleAnywhere, Category = Collision)   //    카테고리는 언제 쓰는지???  종류는??   BodyMesh,
	class USphereComponent* collisionComp;  

	// 메시
	UPROPERTY(VisibleAnywhere, Category = BodyMesh)     
	class UStaticMeshComponent* bodyMeshComp;

	//무브먼트
	UPROPERTY(VisibleAnywhere, Category = Movement)    // #include 안해도됨
	class UProjectileMovementComponent* movementComp;

	// 함수선언 -----------------------------------------------------------------------------------------------
	void Die();    // 총알제거 함수

};
