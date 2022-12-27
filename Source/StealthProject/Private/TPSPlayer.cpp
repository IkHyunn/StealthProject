// Fill out your copyright notice in the Description page of Project Settings.
// 단축키 alt + o




#include "TPSPlayer.h"   // 플레이어
#include <GameFramework/SpringArmComponent.h>   //  스프링암 
#include <Camera/CameraComponent.h>        // 카메라 
#include "Bullet.h"     // 총알
#include <UObject/ConstructorHelpers.h>  // 5- 유탄총 & 스나이퍼건
#include <Blueprint/UserWidget.h>    // 6- 스나이퍼 UI
#include <Kismet/GameplayStatics.h>   // 6- 3  총알퍄편효과 에미터
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>   // 10- 소켓때문?
#include "IH_Enemy.h"
#include "EnemyFSM.h"
#include "../StealthProjectGameModeBase.h"






ATPSPlayer::ATPSPlayer()   // 생성자 함수 등록   --------------------------------------------------------------------------------
{
	PrimaryActorTick.bCanEverTick = true;

	//  1-스켈레탈메쉬 
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Wise/Character/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));

	// 1-성공이면 위치,회전값 고정
	if (TempMesh.Succeeded())    // 
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object); //GetMesh 함수를 이용하여 Mesh 컴포넌트를 가져와 SetSkeletalMesh 함수를 호출.

		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));  //메시컴포넌트 위치, 회전값 설정

	}
	// 테스트용
	//  1-스프링암 
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));  //서브오브젝트 팩토리함수를 이용하여 <  > 인스턴스를 생성하여 변수에 저장
																						 // 변수 =     TEXT("고유이름")	
	springArmComp->SetupAttachment(RootComponent);      // 루트의 자식으로
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));    // 오른쪽 위에서  위치 (변수)
	springArmComp->TargetArmLength = 400;         // 암 길이= 400  (변수)


	//  1-이동
	springArmComp->bUsePawnControlRotation = true;      //  스프링암은 회전시 종속됨 



	// 1-카메라 
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));  // 등록
	tpsCamComp->SetupAttachment(springArmComp);      // 스프링암의 자식
	tpsCamComp->bUsePawnControlRotation = false;      // 카메라는 회전시 자유
	bUseControllerRotationYaw = true;      //  플레이어회전- 클래스의 디폴트 설정값을 의미 

	// 1-더블 점프
	JumpMaxCount = 2;

	// 2-총 메시 
	gunMeshComp = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("GunMeshComp"));    // 등록
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));    //스켈레탈 메시의 자식    10- 소켓붙이기

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/FPWeapon/Mesh/SK_FPGun.SK_FPGun'")); //  데이터 로드 //  TempGunMesh 변수선언
	if (TempGunMesh.Succeeded())  // 만약 성공하면
	{
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);  // 스켈레탈메시 데이터 할당
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 13));    // 총 위치   10- 소켓붙이기로 위치변경
		gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));    // 회전  10- 소켓붙이기
	}
		
	

	//5- 스나이퍼 건 메시

	
	sniperGunComp = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("SniperGunComp"));    // 등록
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));    // 5-메시의 자식, 10- 소켓붙이기

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/Wise/Resources/SniperGun/sniper1.sniper1'")); //  데이터 로드 //  TempSniperMesh 구조체(ConstructorHelpers)변수선언
	
	if (TempSniperMesh.Succeeded())  // 만약 성공하면
	{
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);  // 스태틱메시 데이터 할당
		sniperGunComp->SetRelativeLocation(FVector(-12, 75, 13));   // 10 스나이퍼건 소켓에 붙이기 위치바꾸기
		sniperGunComp->SetRelativeRotation(FRotator(0, 90, 0));     // 10 회전
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));   // 총 스케일 
	}

}






void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	
	ChangeToSniperGun();  // 5-  스나이퍼건을 기본총으로
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      // 6-2 스나이퍼 ui 위젯 인스턴스 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);   // 6-5 일반 조준 ui 크로스헤어 인스턴스를 생성한다 
	_crosshairUI->AddToViewport();       //  일반조준 UI 등록

	HP = initialHP;
		
}

void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();    // 무브 호출 -

	currentTime += DeltaTime;  // 현재 시간에 DeltaTime 누적
	
}




// 함수 내용 -------------------------------------------------------------------------------------------------------------------

void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); // 실행될 때 딱 한번 호출된다- 함수와 입력키와 연결(바인딩)해 준다

	// 2-1. 바인딩 
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);  //인풋컴포넌트야   내게 있는  함수주소(Turn) 를 넘겨주어 처리하도록 연결해 준다
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::InputVertical);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::InputJump);   // 점프 
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);   // 3- 발사 
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &ATPSPlayer::ChangeToGrenadeGun);   // 5- 그레나데건
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToSniperGun);   // 5- 스나이퍼건
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);    //6- 스나이퍼 pressed
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);    //6- 스나이퍼 Released

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &ATPSPlayer::InputAttack);  // 공격

	PlayerInputComponent->BindAction(TEXT("Assasinate"), IE_Released, this, &ATPSPlayer::InputAssasinate);  // 암살
}

	// 2-1 좌우회전 turn 입력 함수 내용 - yaw 회전 ok
void ATPSPlayer::Turn(float value)
{
	AddControllerYawInput(value);  
}

	// 2-1 상하회전 LookUp  입력 함수 내용 - pitch ok
void ATPSPlayer::LookUp(float value)
{
	AddControllerPitchInput(value); 
}
void ATPSPlayer::InputHorizontal(float value)  // 좌우이동 함수내용
{
	direction.Y = value;
}
void ATPSPlayer::InputVertical(float value)     // 상하이동 함수내용
{
	direction.X = value;
}
void ATPSPlayer::InputJump()      // 점프 함수 내용- () !!!
{
	Jump();     // 점프 함수 내용: ACharacter 함수에 이미 점프함수가 잇어 호출만 하면 됨
}
void ATPSPlayer::Move()         // 무브 함수로 빼기
{

	// 가는 방향으로 바라보기-
	direction = FTransform(GetControlRotation()).TransformVector(direction);   // 캐릭터 무브먼트 컴포넌트의 활용, walkspeed 필요없음
			 // 로테이션값을 받아와 direction을 바꾸겟다 = 이동방향을 컨트롤방향 기준으로 변환한다, 
				//이동  P = P0 + vt
				//FVector P0 = GetActorLocation();
				//FVector vt = direction * walkSpeed * DeltaTime;
				//FVector P = P0 + vt;
				//SetActorLocation(P);

	AddMovementInput(direction);
	direction = FVector::ZeroVector;   // 방향 제로 세팅   ????
}



void ATPSPlayer::InputFire()   // 3- 발사 
{
	if (bUsingGrenadeGun)  // 6- 유탄총이면 
	{                // 총알발사스폰하고
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));    //3-총구위치 :
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);  // 3-스폰 : 
	}
	else     // 스나이퍼건 사용하면
	{
		FVector startPos = tpsCamComp->GetComponentLocation();  // 라인트레이스의 시작 위치
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;  // 라인트레이스의 종료위치
		FHitResult hitInfo;       // 라인트레이스의 충돌정보를 담을 변수
		FCollisionQueryParams params;     //  충돌 옵션 설정 변수
		params.AddIgnoredActor(this);      // 자기 자신(플레이어)는 충돌에서 제외
		// 채널 필터를 이용한 라인트레이스 충돌 검출(충돌정보,시작위치, 종료위치,검출채널,충돌옵션)해서 충돌이 성공하면 변수에 true값이 들어온다
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		// 만약에 라인트레이스가 부딪혔을 때 충돌처리 후 총알 파편효과를 재생한다
		if (bHit)
		{
			FTransform bulletTrans;   // 총알파편 효과 트랜스폼
			bulletTrans.SetLocation(hitInfo.ImpactPoint);     // 부딪힌 위치 할당
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);  // 총알파편효과인스턴스 생성
		
			
			auto hitComp = hitInfo.GetComponent();     //6-4 부딪힌 물체의 컴포넌트 변수를 저장한다
			if (hitComp && hitComp->IsSimulatingPhysics())   //만약 컴포넌트에 물리가 적용되어 있다면
			{
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;   // 2.날려버릴 힘과 방향이 필요
				hitComp->AddForce(force);    //3. 그 방향으로 날려버린다
			}

			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
		}

	}
}

void ATPSPlayer::ChangeToGrenadeGun()    // 5- 그레나데건으로
{
	bUsingGrenadeGun = true;   // 사용중  true
	sniperGunComp->SetVisibility(false);   //스나이퍼 no
	gunMeshComp->SetVisibility(true);    // 그레나데 yes


}

void ATPSPlayer::ChangeToSniperGun()   // 스나이퍼건으로
{
	bUsingGrenadeGun = false;   // 사용중  false
	sniperGunComp->SetVisibility(true);   //스나이퍼 yes
	gunMeshComp->SetVisibility(false);    // 그레나데 no
}

void ATPSPlayer::SniperAim()  // 6- 스나이퍼 입력 눌럿을 때 떼엇을 때
{
	if (bUsingGrenadeGun)    // 유탄총 사용중일 때는 종료한다
	{
		return;
	}
	// pressed 임  :
	if (bSniperAim == false)  
	{
		bSniperAim = true;    // 초기값은 false 였으므로 true 로 바꾸고
		_sniperUI->AddToViewport();     // UI 화면에 보이고
		tpsCamComp->SetFieldOfView(45.0f);     // 카메라는 45도 각도
		_crosshairUI->RemoveFromParent();  //  6- 일반조준 ui 안보이게
	}
	// released 임  
	else
	{
		bSniperAim = false;     // 
		_sniperUI->RemoveFromParent();    // ui 안보이게
		tpsCamComp->SetFieldOfView(90.0f);   //카메라 시야각 원래대로 90
		_crosshairUI->AddToViewport();     //6- 일반조준 ui 등록
	}
	// 빌드후 프로젝트에 있는 source 폴더의 프로젝트.buld.cs 더블클릭 열어서 생성자 안에서 umg 모듈을 추가해 주어야 한다
	//  addRange() "UMG" 추가 
}

void ATPSPlayer::InputAttack()  // 공격하는 함수(마우스 좌클릭)
{
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Enemy::StaticClass());  // AIH_Enemy를 찾아서 actor 변수에 넣어둔다.
	if(actor == nullptr) return;

	AIH_Enemy* enemy = Cast<AIH_Enemy>(actor);  // actor가 AIH_Enemy가 맞으면 enemy 변수에 저장한다.
	UEnemyFSM* enemyFSM = Cast<UEnemyFSM>(enemy);

	
	if (currentTime > attackDelayTime)
	{
		enemy->fsm->OnDamageProcess();
		currentTime = 0;
	}
}

void ATPSPlayer::InputAssasinate()  // 암살하는 함수(키보드 E)
{
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Enemy::StaticClass());
	AIH_Enemy* enemy = Cast<AIH_Enemy>(actor);
	UEnemyFSM* enemyFSM = Cast<UEnemyFSM>(enemy);

	if (isBack == true)
	{
		enemy->fsm->OnBackAttack();
		isBack = false;
	}
}

void ATPSPlayer::OnHitEvent()  // 피격 이벤트
{
	UE_LOG(LogTemp, Warning, TEXT("Player Damaged!"));
	HP--;
	UE_LOG(LogTemp, Warning, TEXT("Player HP : %d"), HP);

	if (HP <= 0)  // HP가 0이 되면
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Dead!"));
		OnGameOver();  // 게임 오버 함수 호출
	}
}

void ATPSPlayer::OnGameOver()
{
	AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();  // currMode에 현재 게임모드인 AStealthProjectGameModeBase를 가져온다.

	currMode->ShowGameOverUI();  // 게임모드에서 만든 ShowGameOverUI 함수를 호출한다.
	UGameplayStatics::SetGamePaused(GetWorld(), true);  // 게임을 멈춘다.
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);  // 마우스 커서를 화면에 띄운다.
}