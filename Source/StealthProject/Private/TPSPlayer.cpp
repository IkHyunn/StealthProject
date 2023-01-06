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
#include <GameFramework/CharacterMovementComponent.h>    // 10-
#include <Particles/ParticleSystem.h>
#include "PlayerAnim.h"
#include <Components/BoxComponent.h>






ATPSPlayer::ATPSPlayer()   // 생성자  --------------------------------------------------------------------------------
{
	PrimaryActorTick.bCanEverTick = true;

	// 스켈레탈메쉬 
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Wise/Character/Player/Arissa.Arissa'"));
	if (TempMesh.Succeeded())  
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object); //GetMesh 함수를 이용하여 Mesh 컴포넌트를 가져와 SetSkeletalMesh 함수를 호출.

		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));  //위치, 회전값 
	}

	// 스프링암 
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));  //서브오브젝트 팩토리함수를 이용하여 <  > 인스턴스를 생성하여 변수에 저장
	springArmComp->SetupAttachment(RootComponent);      // 루트의 자식으로
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));    // 위치
	springArmComp->TargetArmLength = 400;         // 암 길이
	springArmComp->bUsePawnControlRotation = true;      //  스프링암은 회전시 종속됨 

	// 1-카메라 
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp")); 
	tpsCamComp->SetupAttachment(springArmComp);      // 스프링암의 자식
	tpsCamComp->bUsePawnControlRotation = false;      // 카메라는 회전시 자유
	bUseControllerRotationYaw = true;      //  플레이어회전- 클래스의 디폴트 설정값을 의미 

	//총  메시
	pistolComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SniperGunComp"));  
	pistolComp->SetupAttachment(GetMesh(), TEXT("Pistol_Socket"));    // 메시의 자식, 소켓붙이기
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempPistolMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Pistols_A.Pistols_A'")); //  데이터 로드 //  TempSniperMesh 구조체(ConstructorHelpers)변수선언
	if(TempPistolMesh.Succeeded())  
	{
		pistolComp->SetSkeletalMesh(TempPistolMesh.Object);  
		pistolComp->SetRelativeLocation(FVector(-8.385662, 3.357624, -2.620080));   
		pistolComp->SetRelativeRotation(FRotator(0, 90, 0));     
	}
	// 칼 메시
	knifeComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("KnifeComp"));
	knifeComp->SetupAttachment(GetMesh(), TEXT("Knife_Socket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempKnifeMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Knife_A.Knife_A'"));
	if (tempKnifeMesh.Succeeded())
	{
		knifeComp->SetSkeletalMesh(tempKnifeMesh.Object);
		knifeComp->SetRelativeLocation(FVector(-8.385662, 3.357624, -2.620080));
		knifeComp->SetRelativeRotation(FRotator(0, 90, 0));
	}
	// 소켓 컬리젼
	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	compBox->SetupAttachment(GetMesh(), TEXT("Hand_Socket"));
	compBox->SetBoxExtent(FVector(20));

	// 1.위젯클래스
	ConstructorHelpers::FClassFinder<UUserWidget> _tempsniperUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_SniperUI.WBP_SniperUI_C'"));
	if (_tempsniperUI.Succeeded())
	{
		sniperUIFactory = _tempsniperUI.Class;
	}
	// 2.크로스헤어 위젯클래스 가져오기
	ConstructorHelpers::FClassFinder<UUserWidget> tempUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_Crosshair.WBP_Crosshair_C'"));
	if (tempUI.Succeeded())
	{
		crosshairUIFactory = tempUI.Class;
	}
	// 3.총알 블루프린트 클래스 가져오기
	ConstructorHelpers::FClassFinder<ABullet> tempbulletFactory(TEXT("Blueprint'/Game/Wise/Blueprints/BP_Bullet.BP_Bullet_C'"));
	if (tempbulletFactory.Succeeded())
	{
		bulletFactory = tempbulletFactory.Class;
	}
	// 4.총파편 파티클 오브젝트 가져오기
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempEffectFactory(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect'"));
	if (tempEffectFactory.Succeeded())
	{
		bulletEffectFactory = tempEffectFactory.Object;
	}
	//UPROPERTY(EditDefaultsOnly, Category = CameraMotion)     //카메라세이크 블루프린트를 저장할 변수
	//	TSubclassOf<class UCameraShakeBase> cameraShake;
	// 5.카메라 쉐이크 블루프린트 가져오기
	ConstructorHelpers::FClassFinder<UCameraShakeBase> tempShakeBase(TEXT("Blueprint'/Game/Wise/Blueprints/BP_CamerShake.BP_CamerShake_C'"));
	if (tempShakeBase.Succeeded())
	{
		cameraShake = tempShakeBase.Class;
	}
	// 6.총 사운드 오브젝트 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase>tempSound(TEXT("SoundWave'/Game/StarterContent/Audio/Explosion01.Explosion01'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
	
	JumpMaxCount = 2;   // 더블 점프
}


void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// 초기설정
	pistolComp->SetVisibility(false);   // 총no  
	knifeComp->SetVisibility(false);   // 칼no
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  //걷기로
	HP = initialHP;   // hp = 0

	anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());   // 처음에 플레이어의 anim 캐스트
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      // 스나이퍼ui위젯생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);    // 크로스헤어위젯생성

	compBox -> OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::OnOverlap); // 소켓컬리젼?
}



void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();   // 이동
	currentTime += DeltaTime;  // 시간누적 

}


void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); 
	//바인딩 
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);  
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::InputVertical);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::InputJump);   // 점프 
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);   // 발사 
	PlayerInputComponent->BindAction(TEXT("NoEquipped"), IE_Pressed, this, &ATPSPlayer::ChangeToNoEquipped);   // 주먹
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToPistol);   // 권총
	PlayerInputComponent->BindAction(TEXT("Knife"), IE_Pressed, this, &ATPSPlayer::ChangeToKnife); // 칼
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);    // 조준 
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);    // 
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATPSPlayer::InputRun); // 달리기
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATPSPlayer::InputRun);   // 걷기
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ATPSPlayer::InputCrouch); // 숙이기
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ATPSPlayer::InputCrouch);
	PlayerInputComponent->BindAction(TEXT("Assasinate"), IE_Released, this, &ATPSPlayer::InputAssasinate);  // 암살
}

void ATPSPlayer::Turn(float value)  
{
	AddControllerYawInput(value);  
}
	
void ATPSPlayer::LookUp(float value) 
{
	AddControllerPitchInput(value); 
}

void ATPSPlayer::InputHorizontal(float value)  
{
	direction.Y = value;
}

void ATPSPlayer::InputVertical(float value)    
{
	direction.X = value;
}

void ATPSPlayer::InputJump()    
{
	Jump();     
}

void ATPSPlayer::Move()      
{
	direction = FTransform(GetControlRotation()).TransformVector(direction);   // 가는 방향바라보기 
	AddMovementInput(direction);
	direction = FVector::ZeroVector;   
}

void ATPSPlayer::InputRun() // MaxWalkSpeed 설정      
{
	auto movement = GetCharacterMovement();
	if (bNoEquipped == true)    // 맨주먹이면 달리고 
	{
		if (movement->MaxWalkSpeed > walkSpeed)
		{
			movement->MaxWalkSpeed = walkSpeed;   //걷는 속도
		}
		else
		{
			movement->MaxWalkSpeed = runSpeed;     //달리는 속도
		}
	}
	else return;
}

void ATPSPlayer::InputCrouch()   // 웅크리기
{
	auto movement = GetCharacterMovement();
	if (bNoEquipped == true)   //주먹
	{
		if (anim->isCrouched == false)
		{
			movement->MaxWalkSpeed = crouchSpeed;
			anim->isCrouched = true;
		}
		else
		{
			movement->MaxWalkSpeed = walkSpeed;
			anim->isCrouched = false;
		}
	}
	else return;
}

void ATPSPlayer::InputFire()  
{
	if (bNoEquipped)  // 주먹질이면
	{              
		if (currentTime > attackDelayTime)  // 일정시간이 지나야 
		{
			anim->isPlayerAttack = true;  // 어택애님 True
			anim->PlayPunchAnim();   // 펀치애님실행
			currentTime = 0; // 시간초기화
			return;
		}
	}
	if ((pistolComp->IsVisible() == true) && (currentBullet > 0))   //권총이고 총알이 잇으면 
	{ 
		anim->PlayAttackAnim();  //총 애님실행
		LineTrace();  // 권총 라인트레이스 
	}
}

void ATPSPlayer::ChangeToNoEquipped()    //주먹 -1번 
{
	//안보이기
	_crosshairUI -> RemoveFromParent(); // 크로스헤어 UI 안보이기
	pistolComp->SetVisibility(false);   // 권총 안보이기
	anim->isGunEquipped = false;       // 권총애님 = no
	knifeComp->SetVisibility(false);   // 칼 안보이기
	                                   // 칼애님 = no
	// 보이기
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  // 속도를 걷는 속도로
	bNoEquipped = true;  // 주먹애님 = yes
}

void ATPSPlayer::ChangeToPistol()   // 권총으로 -2번
{
	if (bgetGun == true)  // 총을 주워야 함수 호출 가능
	{
		bNoEquipped = false;  // 
		GetCharacterMovement()->MaxWalkSpeed=300;  // 걷는 속도
		_crosshairUI->AddToViewport();    // 크로스헤어 보이기
		anim->isGunEquipped = true;  // 권총 든 애니메이션 실행
		pistolComp->SetVisibility(true);   //권총 yes
		knifeComp->SetVisibility(false);   //칼 no
	}
}

void ATPSPlayer::ChangeToKnife()   // 칼 - 3번키 
{
	if (bgetKnife == true)  // 칼 주은 이후와 이후로 3번키 누를 때 함수 호출 가능
	{
		bNoEquipped = false;  // 맨주먹 no
		pistolComp->SetVisibility(false);   //권총 no
		knifeComp->SetVisibility(true);   //칼 yes
		anim->isGunEquipped = false;
		_crosshairUI->RemoveFromParent(); // 크로스헤어 UI 지우기
		                   // 펀치애님실행 // 칼 애니메이션 실행   !!!!
	}
}

void ATPSPlayer::SniperAim()  // 6- 스나이퍼 입력 눌럿을 때 떼엇을 때
{
	if (bNoEquipped == true)   
	{
		return;
	}
	// 
	if (bSniperAim == false)  
	{
		bSniperAim = true;    // 초기값은 false 였으므로 true 로 바꾸고
		_sniperUI->AddToViewport();     
		tpsCamComp->SetFieldOfView(45.0f);    
		_crosshairUI->RemoveFromParent();  //
	}
	//  
	else
	{
		bSniperAim = false;     // 
		_sniperUI->RemoveFromParent();    // ui 안보이게
		tpsCamComp->SetFieldOfView(90.0f);   //카메라 시야각 원래대로 90
		_crosshairUI->AddToViewport();     //6- 일반조준 ui 등록
	}
}


void ATPSPlayer::InputAssasinate()  // 암살하는 함수(키보드 E)
{
	if (backEnemy != nullptr)
	{
		backEnemy->fsm->OnBackAttack();
		backEnemy = nullptr;
	}
}

void ATPSPlayer::fireEffect() 
{
	// 발사 사운드 재생
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);
	// 카메라 세이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();  
	controller->PlayerCameraManager->StartCameraShake(cameraShake);
}

void ATPSPlayer::LineTrace()  //총 발사 라인트레이스
{
	FVector startPos = tpsCamComp->GetComponentLocation();  // 시작 위치
	FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;  // 종료위치
	// FHitResult hitInfo;       //충돌정보  - 전역변수로??
	FCollisionQueryParams params;     //  충돌옵션
	params.AddIgnoredActor(this);      // 자기자신제외

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);  
	
	FTransform bulletTrans;   // 총알파편 효과 
	bulletTrans.SetLocation(hitInfo.ImpactPoint);     // 부딪힌 위치 할당  ???
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);  // 이펙트스폰

	if (bHit)
	{
		fireEffect();  // 사운드 & 카메라세이크 
		currentBullet--;  //총알감소

		// 날려보내기
		auto hitComp = hitInfo.GetComponent();
		if (hitComp && hitComp->IsSimulatingPhysics())   
		{
			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;  
			hitComp->AddForce(force);    
		}

		auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));   //  FSM 이 부딪혓으면 적
		if (enemy)
		{
			auto enemyFSM = Cast<UEnemyFSM>(enemy);
			enemyFSM->OnDamageProcess();
		}
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

void ATPSPlayer::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		AIH_Enemy* enemy = Cast<AIH_Enemy>(OtherActor);

		if (enemy != nullptr)
		{
			if (anim->isPlayerAttack == true)
			{
				UE_LOG(LogTemp, Warning, TEXT("Punch"));
				enemy->fsm->OnDamageProcess();
			}
		}
	}
}