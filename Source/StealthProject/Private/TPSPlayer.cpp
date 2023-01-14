

#include "TPSPlayer.h"  
#include <GameFramework/SpringArmComponent.h>  
#include <Camera/CameraComponent.h>       
#include "Bullet.h"     
#include <UObject/ConstructorHelpers.h>  
#include <Blueprint/UserWidget.h>    
#include <Kismet/GameplayStatics.h>   
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h> 
#include "IH_Enemy.h"
#include "EnemyFSM.h"
#include "../StealthProjectGameModeBase.h"
#include <GameFramework/CharacterMovementComponent.h>  
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
		pistolComp->SetRelativeLocation(FVector(0.218060, -9.718926, 1.827586));
		//(Pitch = 80.000000, Yaw = 449.999999, Roll = 270.000000)
		pistolComp->SetRelativeRotation(FRotator(80.000000, 449.999999, 270.000000));
	}

	// 활 메시
	bowComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("bowComp"));
	bowComp->SetupAttachment(GetMesh(), TEXT("LeftbowSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempbowMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/Bows/source/Bows_and_CrossBows_SketchFab_Ready_.Bows_and_CrossBows_SketchFab_Ready__Recurve Bow 2'"));
	if (tempbowMesh.Succeeded())
	{
		bowComp->SetSkeletalMesh(tempbowMesh.Object);
		bowComp->SetRelativeLocation(FVector(4.504357, -184.203023, -65.049796));
		bowComp->SetRelativeRotation(FRotator(19.987793, 93.703905, -50.103690));

	}

	// 칼 메시
	kalComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("kalComp"));
	kalComp->SetupAttachment(GetMesh(), TEXT("Kal_Socket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempkalMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Knife_A.Knife_A'"));
	if (tempkalMesh.Succeeded())
	{
		kalComp->SetSkeletalMesh(tempkalMesh.Object);
		kalComp->SetRelativeLocation(FVector(-6.101434, -9.099645, 1.452023));
		kalComp->SetRelativeRotation(FRotator(-48.590378, -40.893394, 49.106605));
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
	//UPROPERTY(EditDefaultsOnly, Category = CameraMotion)   
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
	bowComp->SetVisibility(false);   // 활no
	kalComp->SetVisibility(false);   // 칼no
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  //걷기로
	HP = initialHP;   // hp = 0
	anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());  // 플레이어애님cast 
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      //스나이퍼 ui 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);    // 크로스헤어 ui 생성 
	_crosshairUI->RemoveFromParent();  // 크로스헤어 안보이게
	compBox -> OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::OnOverlap);  // 오버랩
}



void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();   // 이동
	currentTime += DeltaTime;  // 시간누적  필요한가??
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
	PlayerInputComponent->BindAction(TEXT("Punch"), IE_Pressed, this, &ATPSPlayer::ChangeToPunch);   // 1번 주먹
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToPistol);   // 권총 2번
	PlayerInputComponent->BindAction(TEXT("Bow"), IE_Pressed, this, &ATPSPlayer::ChangeToBow); // 활 3번
	PlayerInputComponent->BindAction(TEXT("Kal"), IE_Pressed, this, &ATPSPlayer::ChangeToKal); // 칼 4번
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);    // 조준 
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);    
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
	direction = FVector::ZeroVector;   // direction 초기화
}

void ATPSPlayer::InputRun() // MaxWalkSpeed 설정      
{
	auto movement = GetCharacterMovement();
		if (movement->MaxWalkSpeed > walkSpeed)
		{
			movement->MaxWalkSpeed = walkSpeed;   //걷는 속도
		}
		else
		{
			movement->MaxWalkSpeed = runSpeed;     //달리는 속도
		}
}

void ATPSPlayer::InputCrouch()   // 웅크리기
{
	auto movement = GetCharacterMovement();
	if (bPunch == true)   //주먹일때  anim-> isCrouched 는 true
	{
		if (anim->isCrouched == false)    // 눌럿을 때 맨주먹이며 웅크리면
		{
			movement->MaxWalkSpeed = crouchSpeed;  // 200 으로 느리게 걷기
			anim->isCrouched = true;
		}
		else                             // 떼엇을 때
		{
			movement->MaxWalkSpeed = walkSpeed;
			anim->isCrouched = false;
		}
	}
	else return;
}

void ATPSPlayer::InputFire()  
{
	// 펀치이면= 1번이 눌러졌을 때만 왼쪽 마우스로 펀치하고 다른 번호를 눌러야 나간다
	if (bPunch)
	{              
		anim->isPunch = true;  
		anim->PlayPunchAnim();   // 펀치애님실행하기
	}
	
	// 2,3,4 를 눌럿으면 1번 초기화
	if ((pistolComp->IsVisible() == true) || (bowComp->IsVisible() == true) || (kalComp->IsVisible() == true))
	{
		bPunch = false;
	}

	// 칼이면
	if (kalComp->IsVisible() == true)
	{
		anim->PlayKalAimAnim();  //칼 애님 실행 
	}

	//권총이고 총알이 잇으면
	if ((pistolComp->IsVisible() == true) && (currentBullet > 0))    
	{ 
		anim->PlayAttackAnim();  //총 애님 실행
		LineTrace();  // 권총 라인트레이스 
	}

	// 활이면
	if (bowComp->IsVisible() == true)
	{
		anim->PlayBowAimAnim();  //활 애님 실행 
		LineTrace();  // 권총 라인트레이스 
	}
}


void ATPSPlayer::ChangeToPunch()    //주먹 -1번 
{
	//보이기 여부
	pistolComp->SetVisibility(false);  
	kalComp->SetVisibility(false);   
	bowComp->SetVisibility(false);   
	_crosshairUI -> RemoveFromParent(); 

	// 애님실행여부
	anim->isPunch = true;  // 주먹애님 = yes
	anim->isGunEquipped = false;    
	anim->isKal = false;  
	anim->isBow = false;  

	//조건위한 변수  
	bPunch = true;   // 왼쪽 마우스(fire)  선별위한 변수
}



void ATPSPlayer::ChangeToPistol()   // 권총으로 -2번
{
	if (bgetGun == true)  // 총을 주워야 함수 호출 가능
	{
		//보이기 여부
		pistolComp->SetVisibility(true);   //권총 yes
		_crosshairUI->AddToViewport();    // 크로스헤어 yes
		kalComp->SetVisibility(false);   
		bowComp->SetVisibility(false);   

		// 애님실행여부
		anim->isGunEquipped = true;  // 권총 yes
		anim->isBow = false;  
		anim->isPunch = false;
		anim->isKal = false;  
	}
}

void ATPSPlayer::ChangeToBow()   // 활 - 3번키 
{
	if (bgetbow == true)  
	{
		//보이기 여부
		bowComp->SetVisibility(true);   //활 yes
		_crosshairUI->AddToViewport();   // 크로스헤어 yes 
		pistolComp->SetVisibility(false);  
		kalComp->SetVisibility(false);   

		// 애님실행여부
		anim->isBow = true;  // 활 yes
		anim->isGunEquipped = false; 
		anim->isKal = false;  
		anim->isPunch = false;
	}
}

void ATPSPlayer::ChangeToKal()   // 칼 - 4번키 
{
	if (bgetKal == true)  
	{
		//보이기 여부
		kalComp->SetVisibility(true);  // 칼 yes
		_crosshairUI->RemoveFromParent();  // 크로스헤어 yes 
		pistolComp->SetVisibility(false);   
		bowComp->SetVisibility(false);   

		// 애님실행여부
		anim->isKal = true;  // 칼 yes
		anim->isBow = false;
		anim->isPunch = false;
		anim->isGunEquipped = false; 
	}
}




void ATPSPlayer::SniperAim()  // 6- 스나이퍼 입력 눌럿을 때 떼엇을 때
{
    // 함수 패스일때 : 주먹, 칼 
	//if (bPunch == true  ||  bgetKal == true)
	//{
	//	return;
	//}
	// 활 , 총 일 때  
	if ((pistolComp->IsVisible() == true) || (bowComp->IsVisible() == true))
	{
		if (bSniperAim == false)  
		{
			_sniperUI->AddToViewport();   // 스나이퍼 보임
			tpsCamComp->SetFieldOfView(45.0f);    //줌인
			_crosshairUI->RemoveFromParent();  // 크로스헤어 no
			bSniperAim = true;   //바꾸고
		}
		else    
		{
			_sniperUI->RemoveFromParent();    // 스나이퍼 안보임
			tpsCamComp->SetFieldOfView(90.0f);   //줌아웃
			_crosshairUI->AddToViewport();     // 크로스헤어 yes
			bSniperAim = false;     //바꾸고 
		}

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
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);   // 발사 사운드 재생
	auto controller = GetWorld()->GetFirstPlayerController();    // 카메라 세이크 재생
	controller->PlayerCameraManager->StartCameraShake(cameraShake);
}


void ATPSPlayer::LineTrace()  //총, 활 발사 라인트레이스
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
//		currentBullet--;  //총알감소
		
		auto hitComp = hitInfo.GetComponent();  
		if (hitComp && hitComp->IsSimulatingPhysics())   // 날려보내기  
		{
			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;  
			hitComp->AddForce(force);    
		}

		auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));   //  FSM 이 부딪쳤으면 적
		if (enemy)
		{
			auto enemyFSM = Cast<UEnemyFSM>(enemy);     // 애너미fsm 캐스트
			enemyFSM->OnDamageProcess();
		}
	}
}




void ATPSPlayer::OnHitEvent()  // 피격 이벤트
{
		UE_LOG(LogTemp, Warning, TEXT("Player Damaged!"));
		HP--;
		UE_LOG(LogTemp, Warning, TEXT("Player HP : %d"), HP);


		if (HP <= 0)  // 죽음상태로 
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Dead!"));
			this->PlayAnimMontage(anim->DamageDieMontage, 1.0f, FName(TEXT("Die")));  // 죽음 몽타주 재생
			//anim->PlayDamageAnim(TEXT("Die"));
			if (anim->isDieDone == true)   // Die 섹션이 끝나면 
			{
			OnGameOver();  // 게임 오버 함수 호출
			}
			 
		}
		else  // 피격상태로
		{
			int32 index = FMath::RandRange(0, 1);   // 피격 몽타주 랜덤하게 재생
			FString sectionName = FString::Printf(TEXT("Damage%d"), index);
			this->PlayAnimMontage(anim->DamageDieMontage, 1.0f, FName(*sectionName));
			//anim->PlayDamageAnim(FName(*sectionName));  // 피격 몽타주실행하기
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
		AIH_Enemy* enemy = Cast<AIH_Enemy>(OtherActor);     // 애너미 캐스트한다

		if (enemy != nullptr)
		{
			if (anim->isPunch == true)
			{
				UE_LOG(LogTemp, Warning, TEXT("Punch"));
				enemy->fsm->OnDamageProcess();
			}
		}
	}
}