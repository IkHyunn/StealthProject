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

	void IdleState();  // 대기 상태
	void MoveState();  // 이동 상태
	void ChaseState();  // 쫓는 상태
	void AttackState();	// 공격 상태
	void DamageState();	// 피격 상태
	void DieState();  // 죽음 상태
	void ReturnState();  // 제자리로 돌아가는 상태
	void LookState();

	void OnDamageProcess();
	void OnBackAttack();

	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest);	// 랜덤 위치 가져오기

	void IsTargetTrace(FVector start, FVector end, EEnemyState s1, EEnemyState s2);  // LineTrace

public:
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelayTime = 3;	// 대기 시간
	float currentTime = 0;		// 경과 시간
	float moveDelayTime = 1;	// 움직임 대기 시간
	float lookDelayTime = 4;  // 둘러보는 시간

	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* target;		// 플레이어를 타겟으로 설정

	UPROPERTY()
	class AIH_Enemy* me;			// 현재 액터를 나로 설정

	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 150;

	// 시야각을 구하기 위한 변수
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
	float detectedRange = 1000;  // 플레이어를 감지하는 거리

	UPROPERTY(EditAnywhere, Category = FSM)
	float moveRange = 2000;  // 적의 반경

	UPROPERTY(EditAnywhere)
	FVector originPos;  // 적의 처음 위치

	// 라인트레이스 설정을 위한 변수
	FVector startEyePos;  // 눈높이 라인트레이스 시작 위치
	FVector endEyePos;  // 눈높이 라인트레이스 종료 위치
	FVector startSpinePos;  // 골반높이 라인트레이스 시작 위치
	FVector endSpinePos;  // 골반높이 라인트레이스 종료 위치
	FHitResult hitInfo;  // LineTrace의 충돌 정보를 담을 변수
	FCollisionQueryParams params;  // 충돌 옵션 설정 변수

	UPROPERTY(EditAnywhere, Category = FSM)
	float attackDelayTime = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)  // 체력
	int32 HP = 5;

	UPROPERTY(EditAnywhere, Category=FSM)
	float damageDelayTime = 1.5;

	UPROPERTY(EditAnywhere, Category=FSM)
	float dieSpeed = 50;

	UPROPERTY()
	class UEnemyAnim* anim;

	UPROPERTY()
	class AAIController* ai;	// Enemy를 소유하고 있는 AIController 변수 선언

	FVector randomPos;	// 길 찾기 수행 시 랜덤 위치 변수 선언
};
