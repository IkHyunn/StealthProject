// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Chase,
	Attack,
	Damage,
	Die,
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STEALTHPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
		EEnemyState mState = EEnemyState::Idle;

	void IdleState();		// ��� ����
	void MoveState();		// �̵� ����
	void ChaseState();  // �Ѵ� ����
	void AttackState();	// ���� ����
	void DamageState();	// �ǰ� ����
	void DieState();		// ���� ����

	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelayTime = 3.0f;	// ��� �ð�
	float currentTime = 0.0f;		// ��� �ð�
	float moveDelayTime = 1.0f;	// ������ ��� �ð�

	UPROPERTY(VisibleAnywhere, Category = FSM)
		class ATPSPlayer* target;		// �÷��̾ Ÿ������ ����

	UPROPERTY()
		class AIH_Enemy* me;			// ���� ���͸� ���� ����

	UPROPERTY(EditAnywhere, Category = FSM)
		float attackRange = 120.0f;

	// �þ߰��� ���ϱ� ���� ����
	UPROPERTY()
	FVector forward;

	UPROPERTY()
	FVector targetforward;

	float Dot;
	float AcosAngle;
	float AngleDegree;

	FVector OutterProduct;
	float DegSign;
	float ResultDegree;

	UPROPERTY(EditAnywhere, Category = FSM)
		float detectedRange = 1000.0f;

	// ����Ʈ���̽� ������ ���� ����
	FVector startPos;  // ����Ʈ���̽� ���� ��ġ
	FVector endPos;  // ����Ʈ���̽� ���� ��ġ
	FHitResult hitInfo;  // LineTrace�� �浹 ������ ���� ����
	FCollisionQueryParams params;  // �浹 �ɼ� ���� ����
	
	UPROPERTY()
	class AActor* HitActor;

	UPROPERTY(EditAnywhere)
	bool bHit;

	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDelayTime = 2.0f;

	void OnDamageProcess();

	void OnBackAttack();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)  // ü��
		int32 HP = 5;

	UPROPERTY(EditAnywhere, Category=FSM)
		float damageDelayTime = 0.5f;

	UPROPERTY(EditAnywhere, Category=FSM)
		float dieSpeed = 50.0f;

	UPROPERTY()
	class UEnemyAnim* anim;

	UPROPERTY()
	class AAIController* ai;	// Enemy�� �����ϰ� �ִ� AIController ���� ����

	FVector randomPos;	// �� ã�� ���� �� ���� ��ġ ���� ����

	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest);	// ���� ��ġ ��������
};
