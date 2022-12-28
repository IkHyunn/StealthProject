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
	forward = me->GetActorForwardVector();  // ���� ���� �ٶ󺸴� Vector
	targetforward = target->GetActorLocation() - me->GetActorLocation();  // ���� Ÿ���� �ٶ󺸴� Vector
//	UE_LOG(LogTemp, Warning, TEXT("%f"), targetforward.Size());

	Dot = FVector::DotProduct(forward, targetforward.GetSafeNormal());  // GetSafeNormal()�� ������ �ٲ��� �ʰ�, Normalize�� ������ �ٲ۴�.
	AcosAngle = FMath::Acos(Dot);
	AngleDegree = FMath::RadiansToDegrees(AcosAngle);

	// 	FVector OutterProduct = FVector::CrossProduct(forward, targetforward);  // ��, �� ����
	// 	float DegSign = UKismetMathLibrary::SignOfFloat(OutterProduct.Z);
	// 	float ResultDegree = AngleDegree*DegSign;

	// LineTrace ����
	startPos = me->GetActorLocation();  // LineTrace ���� ��ġ
	endPos = me->GetActorLocation() + targetforward;  // LineTrace ���� ��ġ(���� �÷��̾ �ٶ󺸴� ����)
	params.AddIgnoredActor(me);  // �ڱ� �ڽ�(��)�� �浹���� ����

	bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);  // LineTrace �浹 ����, �浹�ϸ� true�� ��ȯ��.
	HitActor = hitInfo.GetActor();  // �浹�� ���͸� ������ HitActor�� ����.
}

void UEnemyFSM::IdleState()
{

	UE_LOG(LogTemp, Warning, TEXT("IDLE"));
	currentTime+=GetWorld()->DeltaTimeSeconds;

	if (currentTime > idleDelayTime)  // ������ �ð��� DelayTime(2)���� ũ�ٸ�
	{
		if (targetforward.Size() < 1000)  // ���� �÷��̾ �ٶ󺸴� �Ÿ��� 1000 �����̸�
		{
			if (AngleDegree < 40)  // ���� ���� �ٶ󺸴� ����� ���� �÷��̾ �ٶ󺸴� ������ ���̰��� -40�� �̻� 40�� �����̸�
			{
				if (bHit)  // LineTrace�� �浹�� �Ǿ��ٸ�
				{
					if (HitActor->GetName().Contains(TEXT("Player")))  // �浹�� Actor�� �̸��� Player�� ���ٸ�
					{
						UE_LOG(LogTemp, Warning, TEXT("Player Detected"));
						mState = EEnemyState::Chase;  // Chase ���·� ��ȯ
						currentTime = 0;  // ������ �ð��� �ʱ�ȭ

						anim->animState = mState;  // �ִϸ��̼� ����ȭ
 					}
					
					else  // �浹�� Actor�� �̸��� Player�� ���� �ʴ´ٸ�
					{
						UE_LOG(LogTemp, Warning, TEXT("%s"), *(HitActor->GetName()));
 						mState = EEnemyState::Move;  // Move ���·� ��ȯ
  						currentTime = 0;  // ������ �ð��� �ʱ�ȭ
  
  						anim->animState = mState;  // �ִϸ��̼� ����ȭ
						GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
					}
				}
			}
		}
		else  // ���� �÷��̾ �ٶ󺸴� �Ÿ��� 1000���� ũ�ٸ�
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *(HitActor->GetName()));
			mState = EEnemyState::Move;  // Move ���·� ��ȯ
			currentTime = 0;  // ������ �ð��� �ʱ�ȭ

			anim->animState = mState;  // �ִϸ��̼� ����ȭ
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}

// 	currentTime += GetWorld()->DeltaTimeSeconds;	// 1. �ð��� �귶���ϱ�
// 	if (currentTime > idleDelayTime)	// 2. ���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
// 	{
// 		mState = EEnemyState::Move;		// 3. �̵����·� ��ȯ�Ѵ�.
// 		currentTime = 0;				// 4. ��� �ð��� �ʱ�ȭ.
// 
// 		anim->animState = mState;	// �ִϸ��̼� ���� ����ȭ
// 
// 		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);  // ���� ���� ��ġ �����ֱ�
// 	}
}

void UEnemyFSM::MoveState()
{
	UE_LOG(LogTemp, Warning, TEXT("MOVE"));
//	currentTime += GetWorld()->DeltaTimeSeconds;
	
// 	if (currentTime > moveDelayTime)  // ������ �ð��� DelayTime(1)���� ũ�ٸ�
// 	{
		if (targetforward.Size() < 1000)  // ���� �÷��̾ �ٶ󺸴� �Ÿ��� 1000 �����̸�
		{
			if (AngleDegree < 40)  // ���� ���� �ٶ󺸴� ����� ���� �÷��̾ �ٶ󺸴� ������ ���̰��� -40�� �̻� 40�� �����̸�
			{
				if (bHit)  // LineTrace�� �浹�� �Ǿ��ٸ�
				{
					if (HitActor->GetName().Contains(TEXT("Player")))
					{
						UE_LOG(LogTemp, Warning, TEXT("Player Detected"));
						mState = EEnemyState::Chase;  // Chase ���·� ��ȯ
//						currentTime = 0;  // ���� �ð� �ʱ�ȭ

						anim->animState = mState;  // �ִϸ��̼� ����ȭ
					}
					else  // �浹�� Actor�� �̸��� Player�� ���� �ʴ´ٸ�
					{
						UE_LOG(LogTemp, Warning, TEXT("%s"), *(HitActor->GetName()));
						me->GetCharacterMovement()->MaxWalkSpeed = 200;
						auto result = ai->MoveToLocation(randomPos);
						if (result == EPathFollowingRequestResult::AlreadyAtGoal)
						{
							mState = EEnemyState::Idle;

							anim->animState = mState;
						}
//						me->AddMovementInput(targetforward.GetSafeNormal());
//						ai->MoveToLocation(me->GetActorLocation() + FVector(-200, 0, 0));
					}
				}
			}
		}
		else  // ���� �÷��̾ �ٶ󺸴� �Ÿ��� 1000���� ũ��
		{
			me->GetCharacterMovement()->MaxWalkSpeed = 200;
			auto result = ai->MoveToLocation(randomPos);
			if (result == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				mState = EEnemyState::Idle;

				anim->animState = mState;
			}
		}
//	}
}

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


void UEnemyFSM::ChaseState()
{
	UE_LOG(LogTemp, Warning, TEXT("Chase"));

	me->GetCharacterMovement()->MaxWalkSpeed = 600;
 	me->AddMovementInput(targetforward.GetSafeNormal());  // 3. �ٶ󺸴� �������� �̵��Ѵ�.
 	ai->MoveToLocation(target->GetActorLocation());

	// Ÿ��� ��������� ���� ���·� ��ȯ
	if (targetforward.Size() < attackRange)	// 1. ���� �Ÿ��� ���ݹ��� ������ ������
											// Size() �Լ��� ũ�⸦ �������� �Լ�.
	{
		ai->StopMovement();

		mState = EEnemyState::Attack;	// 2. ���� ���·� ��ȯ�Ѵ�.

		anim->animState = mState;	// �ִϸ��̼� ���� ����ȭ
		anim->bAttackPlay = true;	// ���� �ִϸ��̼� ��� Ȱ��ȭ
		currentTime = attackDelayTime;	// ���� ���� ��ȯ �� ��� �ð��� �ٷ� �������� ó��
	}
}
void UEnemyFSM::AttackState()
{
	// ���� �ð����� �ѹ��� ����
	currentTime += GetWorld()->DeltaTimeSeconds;	// 1. �ð��� �귯��
	if (currentTime > attackDelayTime)	// 2. ���� ��� �ð��� ���� �ð��� �Ѿ��ٸ�
	{
		// UE_LOG(LogTemp, Warning, TEXT("ATTACK!"));	// 3. �����Ѵ�.
		currentTime = 0;	// 4. ��� �ð��� �ʱ�ȭ.

		anim->bAttackPlay = true;
		//target->OnHitEvent();
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}

	// Ÿ���� ���� ������ ����� �̵����·� ��ȯ
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());		// 1. FVector::Distance(��ġ, ��ġ) : �� ��ġ ������ �Ÿ��� �����ִ� �Լ�.
																								// Target�� Me ������ �Ÿ��� ���Ѵ�.
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

	if (HP > 0)  // HP�� 0 �̻��̸�
	{
		mState = EEnemyState::Damage;  // �ǰ� ���·� ��ȯ
		UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %d"), HP);

		currentTime = 0;

		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), 0);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	else  // HP�� 0 ���Ϸ� ��������
	{
		mState = EEnemyState::Die;  // ���� ���·� ��ȯ
//		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // ���� ���·� ��ȯ�ϸ� ������ ���� �� �ֵ��� ĸ�� �浹ü ��Ȱ��ȭ
		anim->PlayDamageAnim(TEXT("Die"));
	}

	anim->animState = mState;
	ai->StopMovement();
}

void UEnemyFSM::OnBackAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Assasinated!"));
	HP = 0;

	ai->StopMovement();

	mState = EEnemyState::Die;  // ���� ���·� ��ȯ
//	me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // ���� ���·� ��ȯ�ϸ� ������ ���� �� �ֵ��� ĸ�� �浹ü ��Ȱ��ȭ
	anim->PlayDamageAnim(TEXT("Assasinated"));
//	}
}

void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;  // 1. �ð��� �귶���ϱ�

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

// 	// ������ ��� �Ʒ��� ���������� �ϰ� �ʹ�.
// 	FVector P0 = me->GetActorLocation();  // ���� ��ġ
// 	FVector Vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;  // V(����*�ӵ�)*T(�ð�)
// 	FVector P = P0 + Vt;  // ��ġ = ������ġ + ����*�ӵ�*�ð�
// 
// 	me->SetActorLocation(P);
// 
// 	if (P.Z < -200.0f)  // P(��ġ)�� Z���� -200 ���ϰ� �Ǹ�
// 	{
		me->Destroy();  // �ı��Ѵ�.
		AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();
		currMode->AddScore(1);
/*	}*/
}