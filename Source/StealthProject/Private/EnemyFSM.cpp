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
#include <Components/BoxComponent.h>
#include <Components/WidgetComponent.h>
#include "EnemyHP.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UAnimMontage> tempMontage(TEXT("AnimMontage'/Game/Wise/Animations/Enemy/EnemyDamage_Montage.EnemyDamage_Montage'"));
	if (tempMontage.Succeeded())
	{
		damagedMontage = tempMontage.Object;
	}
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

	currHP = maxHP;
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
	case EEnemyState::AttackDelay:
		AttackDelayState();
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
	anim->isOnHit = false;

	if (TargetTrace(startEyePos, endEyePos) || TargetTrace(startSpinePos, endSpinePos))
	{
		ChangeState(EEnemyState::Chase);
	}
	else
	{
		if(IsDelayComplete(idleDelayTime))
		{
			ChangeState(EEnemyState::Move);
		}
	}
}

void UEnemyFSM::MoveState()
{	
	MoveToRandPos(randomPos);

	if (TargetTrace(startEyePos, endEyePos) || TargetTrace(startSpinePos, endSpinePos))
	{
		ChangeState(EEnemyState::Chase);
	}
}

void UEnemyFSM::ChaseState()
{
	float returnDistance = FVector::Distance(originPos, me->GetActorLocation());  // 최초 위치와 현재 위치 간의 거리

	if (returnDistance > moveRange)  // 최초 위치와 현재 위치의 거리가 반경보다 커지면
	{
		ChangeState(EEnemyState::Return);
	}
	if (targetDirection.Length() < attackRange)	 // 1. 만약 거리가 공격범위 안으로 들어오면
	{
		if(AngleDegree < 45)
		{
			ChangeState(EEnemyState::Attack);
		}
		else
		{
			ChangeState(EEnemyState::Idle);
		}
	}
	else
	{
		ai->MoveToLocation(target->GetActorLocation());
	}
}

void UEnemyFSM::AttackDelayState()
{
	// 일정 시간마다 한번씩 공격
	if (IsDelayComplete(attackDelayTime))	// 2. 만약 경과 시간이 공격 시간을 넘었다면
	{
		if (AngleDegree < 45)
		{
			ChangeState(EEnemyState::Attack);
		}
		else
		{
			ChangeState(EEnemyState::Idle);
		}
	}

	// 타깃이 공격 범위를 벗어나면 쫓는상태로 전환
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());		// FVector::Distance(위치, 위치) : 두 위치 사이의 거리를 구해주는 함수.
																								// Target과 Me 사이의 거리를 구한다.
	if (distance > attackRange)
	{
		ChangeState(EEnemyState::Chase);
	}
}

void UEnemyFSM::AttackState()
{
	ChangeState(EEnemyState::AttackDelay);
}

void UEnemyFSM::OnDamageProcess(float damage)
{
	currHP -= damage;
	me->enemyHPUI->UpdateEnemyHP(currHP, maxHP);

	if (currHP > 0)  // HP가 0 이상이면
	{
		ChangeState(EEnemyState::Damage);
	}
	else  // HP가 0 이하로 떨어지면
	{
		ChangeState(EEnemyState::Die);
		me->compBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		me->PlayAnimMontage(damagedMontage, 1.0f, FName(TEXT("Die")));
	}
}

void UEnemyFSM::OnBackAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Assasinated!"));
	currHP = 0;
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	playerController->SetViewTargetWithBlend(me, 0.5);

	anim->bAttackPlay = false;

	ChangeState(EEnemyState::Die);
	me->PlayAnimMontage(damagedMontage, 1.0f, FName(TEXT("Assasinated")));
}

void UEnemyFSM::DamageState()
{
	if (targetDirection.Length() > 1000)  // 거리가 1000 이상이면
	{	
		ChangeState(EEnemyState::Look);
	}
	else
	{
		if (IsDelayComplete(damageDelayTime))
		{
			ChangeState(EEnemyState::Chase);
		}
	}
}

void UEnemyFSM::DieState()
{
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
	MoveToRandPos(originPos);
}

void UEnemyFSM::LookState()
{
	if (isLooking)
	{
		FVector eyeForwardPos = startEyePos + me->compEye->GetForwardVector() * 1500;
		FVector SpineForwardPos = startSpinePos + me->compSpine->GetForwardVector() * 1500;

		FHitResult eyehitInfo;
		FHitResult spinehitInfo;

		bool bEyeHit = GetWorld()->LineTraceSingleByChannel(eyehitInfo, startEyePos, eyeForwardPos, ECC_Visibility, params);
		//DrawDebugLine(GetWorld(), startEyePos, eyeForwardPos, FColor::Red, false, 1.0f, 0, 1.0f);

		if (bEyeHit)
		{
			if (eyehitInfo.GetActor()->GetName().Contains(TEXT("Player")))
			{
				ChangeState(EEnemyState::Chase);
			}
		}

		bool bSpineHit = GetWorld()->LineTraceSingleByChannel(spinehitInfo, startSpinePos, SpineForwardPos, ECC_Visibility, params);
		//DebugLine(GetWorld(), startSpinePos, SpineForwardPos, FColor::Red, false, 1.0f, 0, 1.0f);

		if (bSpineHit)
		{
			if (spinehitInfo.GetActor()->GetName().Contains(TEXT("Player")))
			{
				ChangeState(EEnemyState::Chase);
			}
		}
	}

	if (IsDelayComplete(lookDelayTime))
	{
		ChangeState(EEnemyState::Idle);
	}
}

bool UEnemyFSM::IsDelayComplete(float delaytime)
{
	currentTime += GetWorld()->GetDeltaSeconds();

	if (currentTime > delaytime)
	{
		currentTime = 0;
		return true;
	}
	else return false;
}

void UEnemyFSM::ChangeState(EEnemyState state)
{
	UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState"), true);
	if (enumPtr != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s -> %s"), *(enumPtr->GetNameStringByIndex((int32)mState)), *(enumPtr->GetNameStringByIndex((int32)state)));
	}

	mState = state;  // 현재 상태를 매개변수로 갱신
	anim->animState = state;  // 애니메이션 상태를 매개변수로 갱신
	currentTime = 0;  // 현재 시간을 초기화
	anim->bAttackPlay = false;  // 플레이어 공격 애니메이션 재생x
	ai->StopMovement();

	switch (state)
	{
		case EEnemyState::Idle:
			me->compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			isLooking = false;
			break;
		case EEnemyState::Move:
		{
			me->compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			me->GetCharacterMovement()->MaxWalkSpeed = 200;
			
			UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
			FNavLocation loc;
			ns->GetRandomReachablePointInRadius(originPos, 1000, loc);
			randomPos = loc.Location;
			break;
		}
		case EEnemyState::Damage:
		{
			me->compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			me->SetActorRotation(UKismetMathLibrary::MakeRotFromXZ(target->GetActorLocation() - me->GetActorLocation(), FVector::UpVector));
			int32 index = FMath::RandRange(0, 1);
			FString sectionName = FString::Printf(TEXT("Damage%d"), index);
			me->PlayAnimMontage(damagedMontage, 1.0f, FName(*sectionName));
			break;
		}
		case EEnemyState::AttackDelay:
			me->compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			break;
		case EEnemyState::Attack:
			//me->compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
			break;
		case EEnemyState::Chase:
			me->compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			me->GetCharacterMovement()->MaxWalkSpeed = 600;
			break;
		case EEnemyState::Die:
			me->compHandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
	}
}

// 랜덤 위치에 도착했는지 확인하는 함수
void UEnemyFSM::MoveToRandPos(FVector pos)
{
	EPathFollowingRequestResult::Type result = ai->MoveToLocation(pos);
	
	if (result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		ChangeState(EEnemyState::Idle);
	}
}

// 라인 트레이스 함수
bool UEnemyFSM::TargetTrace(FVector start, FVector end)
{
	if (targetDirection.Length() < 1000 && AngleDegree < 45)
	{
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params);
		//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 1.0f, 0, 1.0f);

		if (bHit)
		{
			if (hitInfo.GetActor()->GetName().Contains(TEXT("Player")))
			{
				return true;
			}
		}
	}
	return false;
}

// void UEnemyFSM::IsTargetTrace(FVector start, FVector end, EEnemyState s1, EEnemyState s2)
//{
// 	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params);
// 	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 1.0f, 0, 1.0f);
// 
// 	if (bHit)  // LineTrace가 충돌했으면
// 	{
// 		if (hitInfo.GetActor()->GetName().Contains(TEXT("Player")))  // 충돌한 액터에 Player라는 글자가 있으면
// 		{
// 			if (s1 == EEnemyState::Chase)  // 매개변수로 받은 Enum 변수가 s1 이면
// 			{
// 				mState = s1;  // Chase 상태로 전환
// 				currentTime = 0;  // 누적 시간 초기화
// 				anim->animState = mState;  // 애니메이션 동기화
// 			}
// 		}
// 		else  // Player라는 글자가 없으면
// 		{
// 			if (s2 == EEnemyState::None)  // State가 None 이면
// 			{
// 				UE_LOG(LogTemp, Warning, TEXT("%s"), *(hitInfo.GetActor()->GetName()));
// 			}
// 			else if (s2 == EEnemyState::Idle)
// 			{
// 				mState = s2;
// 				currentTime = 0;
// 				anim->animState = mState;
// 			}
// 			else if (s2 == EEnemyState::Move)
// 			{
// 				mState = s2;  // Move 상태로 전환
// 				currentTime = 0;  // 누적된 시간을 초기화
// 				anim->animState = mState;  // 애니메이션 동기화
// 			}
// 			else if (s2 == EEnemyState::Look)
// 			{
// 				mState = s2;
// 				currentTime = 0;
// 			}
// 		}
// 	}
//}