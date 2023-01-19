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
	AttackDelay,
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
	void AttackDelayState();

	void OnDamageProcess(float damage);
	void OnBackAttack();

	bool IsDelayComplete(float delaytime);
	void ChangeState(EEnemyState state);
	bool TargetTrace(FVector start, FVector end);
	void MoveToRandPos(FVector pos);

//	void IsTargetTrace(FVector start, FVector end, EEnemyState s1, EEnemyState s2);  // LineTrace

public:
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelayTime = 5;	// ��� �ð�
	float currentTime = 0;		// ��� �ð�
	float moveDelayTime = 1;	// ������ ��� �ð�
	float lookDelayTime = 4;  // �ѷ����� �ð�
	float attackDelayTime = 2;

	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* target;		// �÷��̾ Ÿ������ ����

	UPROPERTY()
	class AIH_Enemy* me;			// ���� ���͸� ���� ����

	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 125;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)  // ü��
	int32 currHP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)  // ü��
	int32 maxHP = 10;

	UPROPERTY(EditAnywhere, Category=FSM)
	float damageDelayTime = 1;

	UPROPERTY(EditAnywhere, Category=FSM)
	float dieSpeed = 50;

	UPROPERTY()
	class UEnemyAnim* anim;

	UPROPERTY()
	class AAIController* ai;	// Enemy�� �����ϰ� �ִ� AIController ���� ����

	UPROPERTY(EditAnywhere)
	class UAnimMontage* damagedMontage;

	FVector randomPos;	// �� ã�� ���� �� ���� ��ġ ���� ����

	bool isDamaged = false;
	bool isLooking = false;
};
