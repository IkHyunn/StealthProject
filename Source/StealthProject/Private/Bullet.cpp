

#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

ABullet::ABullet()      // ������ : ���( ������ �ֱ�) - ������ �Լ����� ������Ʈ���� ���Ϳ� �߰���Ų��
{
	PrimaryActorTick.bCanEverTick = true; 

	// 2- �ݸ��� 
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));    // �ݸ��� �߰� // �浹ü �ν��Ͻ��� ����� ����ϰ� ������ ��Ƴ��´�
	RootComponent = collisionComp;   // ��Ʈ��  //  SetRootComponent(collisionComp)  
	//  collisionComp =DDDD(13);	   //  ũ��  // ????    setboxextent(FVector(        j
	collisionComp->SetSphereRadius(13);
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));    // �������� = BlockAll
		
	// �޽� ������Ʈ
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));     // �޽� �߰�
	bodyMeshComp->SetupAttachment(collisionComp);     // �ڽ�
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));    // �����ũ��
	bodyMeshComp->SetRelativeLocation(FVector(0, 0, -13));
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);	  //  �޽�= no collision

	// �޽�
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (tempMesh.Succeeded())
	{
		bodyMeshComp->SetStaticMesh(tempMesh.Object);
	}

	// �����Ʈ 
	movementComp = CreateDefaultSubobject< UProjectileMovementComponent>(TEXT("MovementComp"));   // m �빮��???  ���� ����
	movementComp->SetUpdatedComponent(collisionComp);      //  �����Ʈ�� ������Ʈ ��ų �ݸ������� ���� (SetUpdatedComponent)
	movementComp->InitialSpeed = 5000;     // �ʱ�ӵ�
	movementComp->MaxSpeed = 5000;     // �ִ�ӵ�
	movementComp->bShouldBounce = true;     // �ݵ� ����
	movementComp->Bounciness = 0.3f;     // �ݵ� ��

	//   InitialLifeSpan = 2.0f;   //  4- �Ѿ��ν��Ͻ� ���� ���1: 2���� �ڵ��Ҹ�


}
	      

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle deathTimer;    // 4- �Ѿ����Ź��2: Ÿ�̸��̿�
	GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::Die, 2.0f, false); // (����,die�Լ�������ü=this,die�Լ��ּ�,2�ʵ���,�ݺ� no)
	 
	// 4- �Ѿ����Ź��3 :�����Լ� : 
	GetWorld()->GetTimerManager().SetTimer(deathTimer, FTimerDelegate::CreateLambda
	(
		[this]()->void
		{
			Destroy();
		}
	), 2.0f, false);
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ����  (p = p0 + vt)
	//FVector p0 = GetActorLocation();  
	//FVector vt = GetActorUpVector() * ddd * DeltaTime;
	//SetActorLocation(p0 + vt);
}
//  �Լ�����   -----------------------------------------------------------------------------------------------------------

void ABullet::Die()   // 4- �Ѿ� ���Ź��2: Ÿ�̸��̿� 
{
	Destroy();
}