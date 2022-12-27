
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


// ���� ���� --------------------------------------------
	//                                        float ddd = 1000;    �ӷº���  -> �����Ʈ�� �ʿ� ����




	// ������Ʈ ���� 
	
	// �ݸ���
	UPROPERTY(VisibleAnywhere, Category = Collision)   //    ī�װ��� ���� ������???  ������??   BodyMesh,
	class USphereComponent* collisionComp;  

	// �޽�
	UPROPERTY(VisibleAnywhere, Category = BodyMesh)     
	class UStaticMeshComponent* bodyMeshComp;

	//�����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Movement)    // #include ���ص���
	class UProjectileMovementComponent* movementComp;

	// �Լ����� -----------------------------------------------------------------------------------------------
	void Die();    // �Ѿ����� �Լ�

};
