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
#include <GameFramework/CharacterMovementComponent.h>

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

	originPos = me->GetActorLocation();
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
	case EEnemyState::Move:
		MoveState();
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
	case EEnemyState::Return:
		ReturnState();
		break;
	case EEnemyState::Look:
		LookState();
		break;
	}

	// 적이 바라보는 방향과 적과 타겟 사이의 방향간의 사이각 구하기
	forwardDirection = me->GetActorForwardVector();  // 적이 앞을 바라보는 Vector
	targetDirection = target->GetActorLocation() - me->GetActorLocation();  // 적이 타겟을 바라보는 Vector

	Dot = FVector::DotProduct(forwardDirection, targetDirection.GetSafeNormal());  // GetSafeNormal()은 원본을 바꾸지 않고, Normalize는 원본을 바꾼다.
	AngleDegree = UKismetMathLibrary::DegAcos(Dot);

// 	OutterProduct = FVector::CrossProduct(targetDirection, targetDirection);  // 좌, 우 구분
// 	DegSign = UKismetMathLibrary::SignOfFloat(OutterProduct.Z);
// 	ResultDegree = AngleDegree*DegSign;

	// LineTrace 설정
	startEyePos = me->compEye->GetSocketLocation(TEXT("EyePerception"));  // 눈 높이 LineTrace 시작 위치
	endEyePos = startEyePos + targetDirection;  // 눈 높이 LineTrace 종료 위치(적이 플레이어를 바라보는 방향)
	startSpinePos = me->compSpine->GetSocketLocation(TEXT("SpinePerception"));  // 골반 높이 LineTrace 시작 위치
	endSpinePos = startSpinePos + targetDirection;  // 골반 높이 LineTrace 종료 위치
	params.AddIgnoredActor(me);  // 자기 자신(적)은 충돌에서 제외
}

void UEnemyFSM::IdleState()
{
	currentTime += GetWorld()->GetDeltaSeconds();
	anim->isOnHit = false;

	if (targetDirection.Length() < 1000 && AngleDegree < 45)
	{
		UE_LOG(LogTemp, Warning, TEXT("%f"), targetDirection.Length());
		UE_LOG(LogTemp, Warning, TEXT("%f"), AngleDegree);
		IsTargetTrace(startEyePos, endEyePos, EEnemyState::Chase, EEnemyState::Move);  // 눈 높이 LineTrace
		IsTargetTrace(startSpinePos, endSpinePos, EEnemyState::Chase, EEnemyState::Move);  // 골반 높이 LineTrace
	}
	else
	{
		if(currentTime > idleDelayTime)
		{
			mState = EEnemyState::Move;
			currentTime = 0;
			anim->animState = mState;  // 애니메이션 동기화
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);  // 최초 랜덤 위치 정해주기
		}
	}
}

void UEnemyFSM::MoveState()
{	
	me->GetCharacterMovement()->MaxWalkSpeed = 200;  // 최고 속도 200으로
	auto result = ai->MoveToLocation(randomPos);  // 랜덤 포지션으로 이동한다.

	if (result == EPathFollowingRequestResult::AlreadyAtGoal)  // 랜덤 포지션에 도착했으면
	{
		mState = EEnemyState::Idle;  // Idle 상태로 전환
		anim->animState = mState;  // 애니메이션 동기화
		currentTime = 0;  // 현재 시간 초기화
	}
	else  // 랜덤 포지션에 도착하지 않았으면
	{
		if (targetDirection.Length() < detectedRange && AngleDegree < 45)
	  	{
			IsTargetTrace(startEyePos, endEyePos, EEnemyState::Chase, EEnemyState::None);  // 눈 높이 LineTrace
			IsTargetTrace(startSpinePos, endSpinePos, EEnemyState::Chase, EEnemyState::None);  // 골반 높이 LineTrace
		}
	}
}

//		내비게이션 길찾기
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

void UEnemyFSM::ChaseState()
{
	me->GetCharacterMovement()->MaxWalkSpeed = 600;
	float returnDistance = FVector::Distance(originPos, me->GetActorLocation());  // 최초 위치와 현재 위치 간의 거리

	anim->isOnHit = false;

	if (returnDistance > moveRange)  // 최초 위치와 현재 위치의 거리가 반경보다 커지면
	{
		mState = EEnemyState::Return;
	}
	// 타깃과 가까워지면 공격 상태로 전환
	else if (targetDirection.Length() < attackRange)	// 1. 만약 거리가 공격범위 안으로 들어오면
	{
		if (AngleDegree < 45)
		{
			ai->StopMovement();

			mState = EEnemyState::Attack;	// 2. 공격 상태로 전환한다.

			anim->animState = mState;	// 애니메이션 상태 동기화
			currentTime = attackDelayTime;	// 공격 상태 전환 시 대기 시간이 바로 끝나도록 처리
		}
		else
		{
			mState = EEnemyState::Idle;

			anim->animState = mState;
		}
	}
	else
	{
		ai->MoveToLocation(target->GetActorLocation());
	}
}

void UEnemyFSM::AttackState()
{
	currentTime += GetWorld()->GetDeltaSeconds();

	// 일정 시간마다 한번씩 공격
	if (currentTime > attackDelayTime)	// 2. 만약 경과 시간이 공격 시간을 넘었다면
	{
		if (AngleDegree < 45)
		{
			anim->bAttackPlay = true;
			anim->isOnHit = true;
			currentTime = 0;
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}

	// 타깃이 공격 범위를 벗어나면 쫓는상태로 전환
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());		// FVector::Distance(위치, 위치) : 두 위치 사이의 거리를 구해주는 함수.
																								// Target과 Me 사이의 거리를 구한다.
	if (distance > attackRange)
	{
		mState = EEnemyState::Chase;
		anim->bAttackPlay = false;
		anim->isOnHit = false;

		anim->animState = mState;
	}
}

void UEnemyFSM::OnDamageProcess()
{
	HP--;
	isDamaged = true;

	if (HP > 0)  // HP가 0 이상이면
	{
		me->SetActorRotation(UKismetMathLibrary::MakeRotFromXZ(target->GetActorLocation() - me->GetActorLocation(), FVector::UpVector));
		UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %d"), HP);
		mState = EEnemyState::Damage;  // 피격 상태로 전환
		currentTime = 0;

		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), index);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	else  // HP가 0 이하로 떨어지면
	{
		mState = EEnemyState::Die;  // 죽음 상태로 전환
		anim->PlayDamageAnim(TEXT("Die"));
	}
}

void UEnemyFSM::OnBackAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Assasinated!"));
	HP = 0;

	anim->bAttackPlay = false;
	anim->isOnHit = false;


	mState = EEnemyState::Die;  // 죽음 상태로 전환
	anim->PlayDamageAnim(TEXT("Die"));
}

void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->GetDeltaSeconds();

	ai->StopMovement();
	anim->bAttackPlay = false;
	anim->isOnHit = false;

	if (targetDirection.Length() > 1000)  // 거리가 1000 이상이면
	{	
		mState = EEnemyState::Look;  // 둘러보는 상태로 전환
		currentTime = 0;
		anim->animState = mState;
	}
	else
	{
		if (currentTime > damageDelayTime)
		{
			mState = EEnemyState::Chase;  // Idle 상태로 전환
			currentTime = 0;  // 현재 시간 초기화
			anim->animState = mState;  // 애니메이션 동기화
		}
	}
}

void UEnemyFSM::DieState()
{
	ai->StopMovement();
	
	if (anim->bDieDone == false)  // 아직 죽음 애니메이션이 끝나지 않았다면
	{
		return;  // 바닥으로 내려가지 않도록 처리
	}
	else
	{
		anim->bAttackPlay = false;
		anim->isOnHit = false;

		me->Destroy();  // 파괴한다.
		AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();
		currMode->AddScore(1);
	}
}

void UEnemyFSM::ReturnState()
{
	anim->bAttackPlay = false;
	anim->isOnHit = false;

	EPathFollowingRequestResult::Type result = ai->MoveToLocation(originPos);
	if (result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		mState = EEnemyState::Idle;
		anim->animState = mState;
	}

// 	FVector originDistance = originPos - me->GetActorLocation();
// 	UE_LOG(LogTemp, Error, TEXT("%f"), originDistance.Length());
// 	if (originDistance.Length() < 40)
// 	{
// 		mState = EEnemyState::Idle;
// 		anim->animState = mState;
// 	}
// 	else
// 	{
// 	
// 		ai->MoveToLocation(originPos);
// 		
// 	}
}

void UEnemyFSM::LookState()
{
	currentTime += GetWorld()->GetDeltaSeconds();
	
	anim->bAttackPlay = false;
	anim->isOnHit = false;
	ai->StopMovement();

	FVector eyeForwardPos = startEyePos + me->compEye->GetForwardVector() * 1000;
	FVector SpineForwardPos = startSpinePos + me->compSpine->GetForwardVector() * 1000;

	IsTargetTrace(startEyePos, eyeForwardPos, EEnemyState::Chase, EEnemyState::None);
	IsTargetTrace(startSpinePos, SpineForwardPos, EEnemyState::Chase, EEnemyState::None);

	if (currentTime > lookDelayTime)
	{
		mState = EEnemyState::Idle;
		currentTime = 0;
		anim->animState=mState;
	}
}

void UEnemyFSM::IsTargetTrace(FVector start, FVector end, EEnemyState s1, EEnemyState s2)
{
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params);  // LineTrace 충돌 검출, 충돌하면 true로 반환됨.
	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 1.0f, 0, 1.0f);  // 눈높이 LineTrace 디버그 라인

	if (bHit)  // LineTrace가 충돌했으면
	{
		if (hitInfo.GetActor()->GetName().Contains(TEXT("Player")))  // 충돌한 액터에 Player라는 글자가 있으면
		{
			if (s1 == EEnemyState::Chase)  // 매개변수로 받은 Enum 변수가 s1 이면
			{
				mState = s1;  // Chase 상태로 전환
				currentTime = 0;  // 누적 시간 초기화
				anim->animState = mState;  // 애니메이션 동기화
			}
		}
		else  // Player라는 글자가 없으면
		{
			if (s2 == EEnemyState::None)  // State가 None 이면
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *(hitInfo.GetActor()->GetName()));
			}
			else if (s2 == EEnemyState::Idle)
			{
				mState = s2;
				currentTime = 0;
				anim->animState = mState;
			}
			else if (s2 == EEnemyState::Move)
			{
				mState = s2;  // Move 상태로 전환
				currentTime = 0;  // 누적된 시간을 초기화
				anim->animState = mState;  // 애니메이션 동기화
				GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);  // 최초 랜덤 위치 정해주기
			}
			else if (s2 == EEnemyState::Look)
			{
				mState = s2;
				currentTime = 0;
			}
		}
	}
}