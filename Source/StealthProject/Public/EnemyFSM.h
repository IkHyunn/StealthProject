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
	Return,
	Look,
	None
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

	void IdleState();  // ��� ����
	void MoveState();  // �̵� ����
	void ChaseState();  // �Ѵ� ����
	void AttackState();	// ���� ����
	void DamageState();	// �ǰ� ����
	void DieState();  // ���� ����
	void ReturnState();  // ���ڸ��� ���ư��� ����
	void LookState();

	void OnDamageProcess();
	void OnBackAttack();

	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest);	// ���� ��ġ ��������

	void IsTargetTrace(FVector start, FVector end, EEnemyState s1, EEnemyState s2);  // LineTrace

public:
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelayTime = 3;	// ��� �ð�
	float currentTime = 0;		// ��� �ð�
	float moveDelayTime = 1;	// ������ ��� �ð�
	float lookDelayTime = 4;  // �ѷ����� �ð�

	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* target;		// �÷��̾ Ÿ������ ����

	UPROPERTY()
	class AIH_Enemy* me;			// ���� ���͸� ���� ����

	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 150;

	// �þ߰��� ���ϱ� ���� ����
	UPROPERTY()
	FVector forwardDirection;

	UPROPERTY()
	FVector targetDirection;

	float Dot;
	float AcosAngle;
	float AngleDegree;

// 	FVector OutterProduct;
// 	float DegSign;
// 	float ResultDegree;

	UPROPERTY(EditAnywhere, Category = FSM)
	float detectedRange = 1000;  // �÷��̾ �����ϴ� �Ÿ�

	UPROPERTY(EditAnywhere, Category = FSM)
	float moveRange = 2000;  // ���� �ݰ�

	UPROPERTY(EditAnywhere)
	FVector originPos;  // ���� ó�� ��ġ

	// ����Ʈ���̽� ������ ���� ����
	FVector startEyePos;  // ������ ����Ʈ���̽� ���� ��ġ
	FVector endEyePos;  // ������ ����Ʈ���̽� ���� ��ġ
	FVector startSpinePos;  // ��ݳ��� ����Ʈ���̽� ���� ��ġ
	FVector endSpinePos;  // ��ݳ��� ����Ʈ���̽� ���� ��ġ
	FHitResult hitInfo;  // LineTrace�� �浹 ������ ���� ����
	FCollisionQueryParams params;  // �浹 �ɼ� ���� ����

	UPROPERTY(EditAnywhere, Category = FSM)
	float attackDelayTime = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)  // ü��
	int32 HP = 5;

	UPROPERTY(EditAnywhere, Category=FSM)
	float damageDelayTime = 1.5;

	UPROPERTY(EditAnywhere, Category=FSM)
	float dieSpeed = 50;

	UPROPERTY()
	class UEnemyAnim* anim;

	UPROPERTY()
	class AAIController* ai;	// Enemy�� �����ϰ� �ִ� AIController ���� ����

	FVector randomPos;	// �� ã�� ���� �� ���� ��ġ ���� ����
};
