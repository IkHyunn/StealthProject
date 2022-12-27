

#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

ABullet::ABullet()      // 생성자 : 등록( 변수에 넣기) - 생성자 함수에서 컴포넌트들을 액터에 추가시킨다
{
	PrimaryActorTick.bCanEverTick = true; 

	// 2- 콜리젼 
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));    // 콜리젼 추가 // 충돌체 인스턴스를 만들어 등록하고 변수에 담아놓는다
	RootComponent = collisionComp;   // 루트로  //  SetRootComponent(collisionComp)  
	//  collisionComp =DDDD(13);	   //  크기  // ????    setboxextent(FVector(        j
	collisionComp->SetSphereRadius(13);
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));    // 프로파일 = BlockAll
		
	// 메시 컴포넌트
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));     // 메시 추가
	bodyMeshComp->SetupAttachment(collisionComp);     // 자식
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));    // 상대적크기
	bodyMeshComp->SetRelativeLocation(FVector(0, 0, -13));
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);	  //  메시= no collision

	// 메시
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (tempMesh.Succeeded())
	{
		bodyMeshComp->SetStaticMesh(tempMesh.Object);
	}

	// 무브먼트 
	movementComp = CreateDefaultSubobject< UProjectileMovementComponent>(TEXT("MovementComp"));   // m 대문자???  내가 지정
	movementComp->SetUpdatedComponent(collisionComp);      //  무브먼트가 업데이트 시킬 콜리젼컴프 지정 (SetUpdatedComponent)
	movementComp->InitialSpeed = 5000;     // 초기속도
	movementComp->MaxSpeed = 5000;     // 최대속도
	movementComp->bShouldBounce = true;     // 반동 여부
	movementComp->Bounciness = 0.3f;     // 반동 값

	//   InitialLifeSpan = 2.0f;   //  4- 총알인스턴스 제거 방법1: 2초후 자동소멸


}
	      

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle deathTimer;    // 4- 총알제거방법2: 타이머이용
	GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::Die, 2.0f, false); // (변수,die함수가진개체=this,die함수주소,2초동안,반복 no)
	 
	// 4- 총알제거방법3 :람다함수 : 
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

	// 전진  (p = p0 + vt)
	//FVector p0 = GetActorLocation();  
	//FVector vt = GetActorUpVector() * ddd * DeltaTime;
	//SetActorLocation(p0 + vt);
}
//  함수내용   -----------------------------------------------------------------------------------------------------------

void ABullet::Die()   // 4- 총알 제거방법2: 타이머이용 
{
	Destroy();
}