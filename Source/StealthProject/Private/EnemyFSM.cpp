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

	ai = Cast<AAIController>(me->GetController());	// ai ������ AIController �Ҵ�

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
	float returnDistance = FVector::Distance(originPos, me->GetActorLocation());  // ���� ��ġ�� ���� ��ġ ���� �Ÿ�

	if (returnDistance > moveRange)  // ���� ��ġ�� ���� ��ġ�� �Ÿ��� �ݰ溸�� Ŀ����
	{
		ChangeState(EEnemyState::Return);
	}
	if (targetDirection.Length() < attackRange)	 // 1. ���� �Ÿ��� ���ݹ��� ������ ������
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
	// ���� �ð����� �ѹ��� ����
	if (IsDelayComplete(attackDelayTime))	// 2. ���� ��� �ð��� ���� �ð��� �Ѿ��ٸ�
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

	// Ÿ���� ���� ������ ����� �Ѵ»��·� ��ȯ
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());		// FVector::Distance(��ġ, ��ġ) : �� ��ġ ������ �Ÿ��� �����ִ� �Լ�.
																								// Target�� Me ������ �Ÿ��� ���Ѵ�.
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

	if (currHP > 0)  // HP�� 0 �̻��̸�
	{
		ChangeState(EEnemyState::Damage);
	}
	else  // HP�� 0 ���Ϸ� ��������
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
	if (targetDirection.Length() > 1000)  // �Ÿ��� 1000 �̻��̸�
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
	if (anim->bDieDone == false)  // ���� ���� �ִϸ��̼��� ������ �ʾҴٸ�
	{
		return;  // �ٴ����� �������� �ʵ��� ó��
	}
	else
	{
		anim->bAttackPlay = false;
		anim->isOnHit = false;

		me->Destroy();  // �ı��Ѵ�.
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

	mState = state;  // ���� ���¸� �Ű������� ����
	anim->animState = state;  // �ִϸ��̼� ���¸� �Ű������� ����
	currentTime = 0;  // ���� �ð��� �ʱ�ȭ
	anim->bAttackPlay = false;  // �÷��̾� ���� �ִϸ��̼� ���x
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

// ���� ��ġ�� �����ߴ��� Ȯ���ϴ� �Լ�
void UEnemyFSM::MoveToRandPos(FVector pos)
{
	EPathFollowingRequestResult::Type result = ai->MoveToLocation(pos);
	
	if (result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		ChangeState(EEnemyState::Idle);
	}
}

// ���� Ʈ���̽� �Լ�
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
// 	if (bHit)  // LineTrace�� �浹������
// 	{
// 		if (hitInfo.GetActor()->GetName().Contains(TEXT("Player")))  // �浹�� ���Ϳ� Player��� ���ڰ� ������
// 		{
// 			if (s1 == EEnemyState::Chase)  // �Ű������� ���� Enum ������ s1 �̸�
// 			{
// 				mState = s1;  // Chase ���·� ��ȯ
// 				currentTime = 0;  // ���� �ð� �ʱ�ȭ
// 				anim->animState = mState;  // �ִϸ��̼� ����ȭ
// 			}
// 		}
// 		else  // Player��� ���ڰ� ������
// 		{
// 			if (s2 == EEnemyState::None)  // State�� None �̸�
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
// 				mState = s2;  // Move ���·� ��ȯ
// 				currentTime = 0;  // ������ �ð��� �ʱ�ȭ
// 				anim->animState = mState;  // �ִϸ��̼� ����ȭ
// 			}
// 			else if (s2 == EEnemyState::Look)
// 			{
// 				mState = s2;
// 				currentTime = 0;
// 			}
// 		}
// 	}
//}