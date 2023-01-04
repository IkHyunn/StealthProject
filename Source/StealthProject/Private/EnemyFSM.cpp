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

	ai = Cast<AAIController>(me->GetController());	// ai ������ AIController �Ҵ�
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
	}

	// ���� �ٶ󺸴� ����� ���� Ÿ�� ������ ���Ⱓ�� ���̰� ���ϱ�
	forwardDirection = me->GetActorForwardVector();  // ���� ���� �ٶ󺸴� Vector
	targetDirection = target->GetActorLocation() - me->GetActorLocation();  // ���� Ÿ���� �ٶ󺸴� Vector

	Dot = FVector::DotProduct(forwardDirection, targetDirection.GetSafeNormal());  // GetSafeNormal()�� ������ �ٲ��� �ʰ�, Normalize�� ������ �ٲ۴�.
	AngleDegree = UKismetMathLibrary::DegAcos(Dot);

// 	OutterProduct = FVector::CrossProduct(targetDirection, targetDirection);  // ��, �� ����
// 	DegSign = UKismetMathLibrary::SignOfFloat(OutterProduct.Z);
// 	ResultDegree = AngleDegree*DegSign;

	// LineTrace ����
	startEyePos = me->compEye->GetSocketLocation(TEXT("EyePerception"));  // �� ���� LineTrace ���� ��ġ
	endEyePos = startEyePos + targetDirection;  // �� ���� LineTrace ���� ��ġ(���� �÷��̾ �ٶ󺸴� ����)
	startSpinePos = me->compSpine->GetSocketLocation(TEXT("SpinePerception"));  // ��� ���� LineTrace ���� ��ġ
	endSpinePos = startSpinePos + targetDirection;  // ��� ���� LineTrace ���� ��ġ
	params.AddIgnoredActor(me);  // �ڱ� �ڽ�(��)�� �浹���� ����
}

void UEnemyFSM::IdleState()
{
	currentTime+=GetWorld()->DeltaTimeSeconds;
	anim->isOnHit = false;

	if (targetDirection.Length() < 1000 && AngleDegree < 45)
	{
		IsTargetTrace(startEyePos, endEyePos, EEnemyState::Chase, EEnemyState::Move);  // �� ���� LineTrace

		IsTargetTrace(startSpinePos, endSpinePos, EEnemyState::Chase, EEnemyState::Move);  // ��� ���� LineTrace
	}
	else
	{
		if (currentTime > idleDelayTime)  // ������ �ð��� DelayTime(2)���� ũ�ٸ�
		{
			mState = EEnemyState::Move;  // Move ���·� ��ȯ
			currentTime = 0;  // ������ �ð��� �ʱ�ȭ

			anim->animState = mState;  // �ִϸ��̼� ����ȭ
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);  // ���� ���� ��ġ �����ֱ�
		}
	}
}

void UEnemyFSM::MoveState()
{	
	me->GetCharacterMovement()->MaxWalkSpeed = 200;  // �ְ� �ӵ� 200����
	auto result = ai->MoveToLocation(randomPos);  // ���� ���������� �̵��Ѵ�.

	if (result == EPathFollowingRequestResult::AlreadyAtGoal)  // ���� �����ǿ� ����������
	{
		mState = EEnemyState::Idle;  // Idle ���·� ��ȯ
		anim->animState = mState;  // �ִϸ��̼� ����ȭ
		currentTime = 0;  // ���� �ð� �ʱ�ȭ
	}
	else  // ���� �����ǿ� �������� �ʾ�����
	{
		if (targetDirection.Length() < 1000 && AngleDegree < 45)
	  	{
			IsTargetTrace(startEyePos, endEyePos, EEnemyState::Chase, EEnemyState::None);  // �� ���� LineTrace
			IsTargetTrace(startSpinePos, endSpinePos, EEnemyState::Chase, EEnemyState::None);  // ��� ���� LineTrace
		}
	}
}

//		������̼� ��ã��
//  	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());  // NavigationSystem ��ü ������
//  	
//   	FPathFindingQuery query;
//   	FAIMoveRequest req;  // ������ �� ã�� ��� ������ �˻�
//  	 
//   	ai->BuildPathfindingQuery(req, query);  // ��ã�⸦ ���� ���� ����
//  	 
//   	FPathFindingResult r = ns->FindPathSync(query);  // ��ã�� ��� ��������
//  
// 
//  	auto result = ai->MoveToLocation(randomPos);  // ���� ��ġ�� �̵�
//  
//  	if (result == EPathFollowingRequestResult::AlreadyAtGoal)  // �������� �����ϸ�
//  	{
//  		mState = EEnemyState::Idle;
//  		anim->animState = mState;
//  		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);  // ���ο� ���� ��ġ�� �����´�.
//  	}

void UEnemyFSM::IsTargetTrace(FVector start, FVector end, EEnemyState s1, EEnemyState s2)
{
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params);  // LineTrace �浹 ����, �浹�ϸ� true�� ��ȯ��.
	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 1.0f, 0, 1.0f);  // ������ LineTrace ����� ����

	if (bHit)  // LineTrace�� �浹������
	{
		if (hitInfo.GetActor()->GetName().Contains(TEXT("Player")))  // �浹�� ���Ϳ� Player��� ���ڰ� ������
		{
			if (s1 == EEnemyState::Chase)  // �Ű������� ���� Enum ������ s1 �̸�
			{
				mState = s1;  // Chase ���·� ��ȯ
				currentTime = 0;  // ���� �ð� �ʱ�ȭ
				anim->animState = mState;  // �ִϸ��̼� ����ȭ
			}
		}
		else  // Player��� ���ڰ� ������
		{
			if (s2 == EEnemyState::None)  // State�� None �̸�
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *(hitInfo.GetActor()->GetName()));
			}
			else if (s2 == EEnemyState::Move)
			{
				mState = s2;  // Move ���·� ��ȯ
				currentTime = 0;  // ������ �ð��� �ʱ�ȭ
				anim->animState = mState;  // �ִϸ��̼� ����ȭ
				GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);  // ���� ���� ��ġ �����ֱ�
			}
		}
	}
}

void UEnemyFSM::ChaseState()
{
	me->GetCharacterMovement()->MaxWalkSpeed = 600;
 	ai->MoveToLocation(target->GetActorLocation());

	// Ÿ��� ��������� ���� ���·� ��ȯ
	if (targetDirection.Size() < attackRange)	// 1. ���� �Ÿ��� ���ݹ��� ������ ������
											// Size() �Լ��� ũ�⸦ �������� �Լ�.
	{
		ai->StopMovement();

		mState = EEnemyState::Attack;	// 2. ���� ���·� ��ȯ�Ѵ�.

		anim->animState = mState;	// �ִϸ��̼� ���� ����ȭ
		anim->bAttackPlay = false;
		currentTime = attackDelayTime;	// ���� ���� ��ȯ �� ��� �ð��� �ٷ� �������� ó��
	}
}

void UEnemyFSM::AttackState()
{
	// ���� �ð����� �ѹ��� ����
	currentTime += GetWorld()->DeltaTimeSeconds;	// 1. �ð��� �귯��

	if (currentTime > attackDelayTime)	// 2. ���� ��� �ð��� ���� �ð��� �Ѿ��ٸ�
	{
		if (targetDirection.Length() < 1000 && AngleDegree < 45)
		{
			anim->bAttackPlay = true;
			anim->isOnHit = true;
			currentTime = 0;
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}
	else
	{
		mState = EEnemyState::Idle;
		currentTime = 0;
		anim->animState = mState;
	}

	// Ÿ���� ���� ������ ����� �Ѵ»��·� ��ȯ
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());		// 1. FVector::Distance(��ġ, ��ġ) : �� ��ġ ������ �Ÿ��� �����ִ� �Լ�.
																								// Target�� Me ������ �Ÿ��� ���Ѵ�.
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

	if (HP > 0)  // HP�� 0 �̻��̸�
	{
		mState = EEnemyState::Damage;  // �ǰ� ���·� ��ȯ
		UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %d"), HP);

		currentTime = 0;

		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), index);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	else  // HP�� 0 ���Ϸ� ��������
	{
		mState = EEnemyState::Die;  // ���� ���·� ��ȯ
		anim->PlayDamageAnim(TEXT("Die"));
	}
}

void UEnemyFSM::OnBackAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Assasinated!"));
	HP = 0;
	anim->isOnHit = false;

	ai->StopMovement();

	mState = EEnemyState::Die;  // ���� ���·� ��ȯ
	anim->PlayDamageAnim(TEXT("Die"));
}

void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;  // 1. �ð��� �귶���ϱ�

	anim->isOnHit = false;
	ai->StopMovement();

	if (currentTime > damageDelayTime)   // 2. ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	{
		mState = EEnemyState::Idle;  // 3. ��� ���·� ��ȯ�ϰ� �ʹ�.
		currentTime = 0;  // 4. �ð��� �����Ǳ� ������ 0���� ��� �ð� �ʱ�ȭ.
		anim->animState = mState;
	}
}

void UEnemyFSM::DieState()
{
	if (anim->bDieDone == false)  // ���� ���� �ִϸ��̼��� ������ �ʾҴٸ�
	{
		return;  // �ٴ����� �������� �ʵ��� ó��
	}

	anim->bAttackPlay = false;

	me->Destroy();  // �ı��Ѵ�.
	AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();
	currMode->AddScore(1);
}