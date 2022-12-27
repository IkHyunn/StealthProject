// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <IH_Enemy.h>
#include <Kismet/GameplayStatics.h>
#include <EnemyAnim.h>
#include <AIController.h>
#include <NavigationSystem.h>
#include <Components/CapsuleComponent.h>
#include "../StealthProjectGameModeBase.h"
#include "TPSPlayer.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	target = Cast<ATPSPlayer>(actor);
	me = Cast<AIH_Enemy>(GetOwner());
	
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());

	ai = Cast<AAIController>(me->GetController());	// ai 변수에 AIController 할당
}

bool UEnemyFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	dest = loc.Location;
	return result;
}

// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Chase:
		ChaseState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}

	// 적이 바라보는 방향과 적과 타겟 사이의 방향간의 사이각 구하기
	forward = me->GetActorForwardVector();  // 적이 앞을 바라보는 Vector
	targetforward = target->GetActorLocation() - me->GetActorLocation();  // 적이 타겟을 바라보는 Vector
//	UE_LOG(LogTemp, Warning, TEXT("%f"), targetforward.Size());

	Dot = FVector::DotProduct(forward, targetforward.GetSafeNormal());  // GetSafeNormal()은 원본을 바꾸지 않고, Normalize는 원본을 바꾼다.
	AcosAngle = FMath::Acos(Dot);
	AngleDegree = FMath::RadiansToDegrees(AcosAngle);

	// 	FVector OutterProduct = FVector::CrossProduct(forward, targetforward);  // 좌, 우 구분
	// 	float DegSign = UKismetMathLibrary::SignOfFloat(OutterProduct.Z);
	// 	float ResultDegree = AngleDegree*DegSign;
}

void UEnemyFSM::IdleState()
{
//	UE_LOG(LogTemp, Warning, TEXT("IDLE"));
	currentTime+=GetWorld()->DeltaTimeSeconds;

	if (currentTime > idleDelayTime)
	{
			if (targetforward.Size() < 1000)
		{
			if (AngleDegree < 30)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player Detected"));
				mState = EEnemyState::Chase;
				currentTime = 0;

				anim->animState = mState;
			}
		}
		else
		{
			mState = EEnemyState::Move;
			currentTime = 0;

			anim->animState = mState;
		}
	}

// 	currentTime += GetWorld()->DeltaTimeSeconds;	// 1. 시간이 흘렀으니까
// 	if (currentTime > idleDelayTime)	// 2. 만약 경과 시간이 대기 시간을 초과했다면
// 	{
// 		mState = EEnemyState::Move;		// 3. 이동상태로 전환한다.
// 		currentTime = 0;				// 4. 경과 시간을 초기화.
// 
// 		anim->animState = mState;	// 애니메이션 상태 동기화
// 
// 		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);  // 최초 랜덤 위치 정해주기
// 	}
}

void UEnemyFSM::MoveState()
{
	UE_LOG(LogTemp, Warning, TEXT("MOVE"));

	if (targetforward.Size() < 1000)
	{
		if (AngleDegree < 30)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Detected"));
			mState = EEnemyState::Chase;
			currentTime = 0;

			anim->animState = mState;
		}
	}

//  	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());  // NavigationSystem 객체 얻어오기
//  	
//   	FPathFindingQuery query;
//   	FAIMoveRequest req;  // 목적지 길 찾기 경로 데이터 검색
//  	 
//   	ai->BuildPathfindingQuery(req, query);  // 길찾기를 위한 쿼리 생성
//  	 
//   	FPathFindingResult r = ns->FindPathSync(query);  // 길찾기 결과 가져오기
//  
// 
//  	auto result = ai->MoveToLocation(randomPos);  // 랜덤 위치로 이동
//  
//  	if (result == EPathFollowingRequestResult::AlreadyAtGoal)  // 목적지에 도착하면
//  	{
//  		mState = EEnemyState::Idle;
//  		anim->animState = mState;
//  		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);  // 새로운 랜덤 위치를 가져온다.
//  	}
}
void UEnemyFSM::ChaseState()
{
	UE_LOG(LogTemp, Warning, TEXT("Chase"));

 	me->AddMovementInput(targetforward.GetSafeNormal());  // 3. 바라보는 방향으로 이동한다.
 	ai->MoveToLocation(target->GetActorLocation());

	// 타깃과 가까워지면 공격 상태로 전환
	if (targetforward.Size() < attackRange)	// 1. 만약 거리가 공격범위 안으로 들어오면
									// Size() 함수는 크기를 가져오는 함수.
	{
		ai->StopMovement();

		mState = EEnemyState::Attack;	// 2. 공격 상태로 전환한다.

		anim->animState = mState;	// 애니메이션 상태 동기화
		anim->bAttackPlay = true;	// 공격 애니메이션 재생 활성화
		currentTime = attackDelayTime;	// 공격 상태 전환 시 대기 시간이 바로 끝나도록 처리
	}
}
void UEnemyFSM::AttackState()
{
	// 일정 시간마다 한번씩 공격
	currentTime += GetWorld()->DeltaTimeSeconds;	// 1. 시간이 흘러서
	if (currentTime > attackDelayTime)	// 2. 만약 경과 시간이 공격 시간을 넘었다면
	{
		// UE_LOG(LogTemp, Warning, TEXT("ATTACK!"));	// 3. 공격한다.
		currentTime = 0;	// 4. 경과 시간을 초기화.

		anim->bAttackPlay = true;
		//target->OnHitEvent();
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}

	// 타깃이 공격 범위를 벗어나면 이동상태로 전환
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());		// 1. FVector::Distance(위치, 위치) : 두 위치 사이의 거리를 구해주는 함수.
																								// Target과 Me 사이의 거리를 구한다.
	if (distance > attackRange)
	{
		mState = EEnemyState::Chase;

		anim->animState = mState;
	}
}

void UEnemyFSM::OnDamageProcess()
{
//	UE_LOG(LogTemp, Warning, TEXT("Enemy Damaged!"));
	HP--;

	if (HP > 0)  // HP가 0 이상이면
	{
		mState = EEnemyState::Damage;  // 피격 상태로 전환
		UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %d"), HP);

		currentTime = 0;

		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), 0);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	else  // HP가 0 이하로 떨어지면
	{
		mState = EEnemyState::Die;  // 죽음 상태로 전환
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 죽음 상태로 전환하면 땅으로 꺼질 수 있도록 캡슐 충돌체 비활성화
		anim->PlayDamageAnim(TEXT("Die"));
	}

	anim->animState = mState;
	ai->StopMovement();
}

void UEnemyFSM::OnBackAttack()
{
//	UE_LOG(LogTemp, Warning, TEXT("Enemy Assasinated!"));
	HP = 0;

// 	if (HP > 0)
// 	{
// 		mState = EEnemyState::Damage;
// 		UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %d"), HP);
// 	}
//	else  // HP가 0 이하로 떨어지면
//	{
	mState = EEnemyState::Die;  // 죽음 상태로 전환
	me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 죽음 상태로 전환하면 땅으로 꺼질 수 있도록 캡슐 충돌체 비활성화
	anim->PlayDamageAnim(TEXT("Die"));
		//me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 죽음 상태로 전환하면 땅으로 꺼질 수 있도록 캡슐 충돌체 비활성화
//	}
}

void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;  // 1. 시간이 흘렀으니까

	if (currentTime > damageDelayTime)   // 2. 경과 시간이 대기 시간을 초과했다면
	{
		mState = EEnemyState::Idle;  // 3. 대기 상태로 전환하고 싶다.
		currentTime = 0;  // 4. 시간이 누적되기 때문에 0으로 경과 시간 초기화.
		anim->animState = mState;
	}
}
void UEnemyFSM::DieState()
{
	if (anim->bDieDone == false)  // 아직 죽음 애니메이션이 끝나지 않았다면
	{
		return;  // 바닥으로 내려가지 않도록 처리
	}

// 	// 죽으면 계속 아래로 내려가도록 하고 싶다.
// 	FVector P0 = me->GetActorLocation();  // 현재 위치
// 	FVector Vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;  // V(방향*속도)*T(시간)
// 	FVector P = P0 + Vt;  // 위치 = 현재위치 + 방향*속도*시간
// 
// 	me->SetActorLocation(P);
// 
// 	if (P.Z < -200.0f)  // P(위치)의 Z값이 -200 이하가 되면
// 	{
		me->Destroy();  // 파괴한다.
		AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();
		currMode->AddScore(1);
/*	}*/
}