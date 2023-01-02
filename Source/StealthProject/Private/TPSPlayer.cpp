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






ATPSPlayer::ATPSPlayer()   // 생성자 함수 등록   --------------------------------------------------------------------------------
{
	PrimaryActorTick.bCanEverTick = true;

	//  1-스켈레탈메쉬 
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Wise/Character/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())  
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object); //GetMesh 함수를 이용하여 Mesh 컴포넌트를 가져와 SetSkeletalMesh 함수를 호출.

		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));  //위치, 회전값 
	}

	//  1-스프링암 
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));  //서브오브젝트 팩토리함수를 이용하여 <  > 인스턴스를 생성하여 변수에 저장
	springArmComp->SetupAttachment(RootComponent);      // 루트의 자식으로
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));    // 위치
	springArmComp->TargetArmLength = 400;         // 암 길이


	//  1-이동
	springArmComp->bUsePawnControlRotation = true;      //  스프링암은 회전시 종속됨 



	// 1-카메라 
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));  // 등록
	tpsCamComp->SetupAttachment(springArmComp);      // 스프링암의 자식
	tpsCamComp->bUsePawnControlRotation = false;      // 카메라는 회전시 자유
	bUseControllerRotationYaw = true;      //  플레이어회전- 클래스의 디폴트 설정값을 의미 

	// 1-더블 점프
	JumpMaxCount = 2;

// 	// 2-총 메시 
// 	gunMeshComp = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("GunMeshComp"));    // 등록
// 	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));    //스켈레탈 메시의 자식    10- 소켓붙이기
// 
// 	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/FPWeapon/Mesh/SK_FPGun.SK_FPGun'")); //  데이터 로드 //  TempGunMesh 변수선언
// 	if (TempGunMesh.Succeeded())  // 만약 성공하면
// 	{
// 		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);  // 스켈레탈메시 데이터 할당
// 		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 13));    // 총 위치   10- 소켓붙이기로 위치변경
// 		gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));    // 회전  10- 소켓붙이기
// 	}
		
	

	//5- 스나이퍼 건 메시

	pistolComp = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("SniperGunComp"));    // 등록
	pistolComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));    // 5-메시의 자식, 10- 소켓붙이기

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempPistolMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Pistols_A.Pistols_A'")); //  데이터 로드 //  TempSniperMesh 구조체(ConstructorHelpers)변수선언
	
	if (TempPistolMesh.Succeeded())  // 만약 성공하면
	{
		pistolComp->SetSkeletalMesh(TempPistolMesh.Object);  // 스태틱메시 데이터 할당
		pistolComp->SetRelativeLocation(FVector(-12, 75, 13));   // 10 스나이퍼건 소켓에 붙이기 위치바꾸기
		pistolComp->SetRelativeRotation(FRotator(0, 90, 0));     // 10 회전
	}


	// 스나이퍼ui 위젯 블루프린트 클래스 가져오기
	ConstructorHelpers::FClassFinder<UUserWidget> _tempsniperUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_SniperUI.WBP_SniperUI_C'"));
	if (_tempsniperUI.Succeeded())
	{
		sniperUIFactory = _tempsniperUI.Class;
	}


	// 크로스헤어 위젯 블루프린트 클래스 가져오기
	ConstructorHelpers::FClassFinder<UUserWidget> tempUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_Crosshair.WBP_Crosshair_C'"));
	if (tempUI.Succeeded())
	{
		crosshairUIFactory = tempUI.Class;
	}


	//  총알 블루프린트 클래스 가져오기
	ConstructorHelpers::FClassFinder<ABullet> tempbulletFactory(TEXT("Blueprint'/Game/Wise/Blueprints/BP_Bullet.BP_Bullet_C'"));
	if (tempbulletFactory.Succeeded())
	{
		bulletFactory = tempbulletFactory.Class;
	}

	// 총파편 파티클 오브젝트 가져오기
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempEffectFactory(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect'"));
	if (tempEffectFactory.Succeeded())
	{
		bulletEffectFactory = tempEffectFactory.Object;
	}

	//UPROPERTY(EditDefaultsOnly, Category = CameraMotion)     //카메라세이크 블루프린트를 저장할 변수
	//	TSubclassOf<class UCameraShakeBase> cameraShake;

	// 카메라 쉐이크 블루프린트 가져오기
	ConstructorHelpers::FClassFinder<UCameraShakeBase> tempShakeBase(TEXT("Blueprint'/Game/Wise/Blueprints/BP_CamerShake.BP_CamerShake_C'"));
	if (tempShakeBase.Succeeded())
	{
		cameraShake = tempShakeBase.Class;
	}


	// 총 사운드 오브젝트 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase>tempSound(TEXT("SoundWave'/Game/StarterContent/Audio/Explosion01.Explosion01'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	compBox ->SetupAttachment(GetMesh(), TEXT("hand_Box_rSocket"));
	compBox ->SetBoxExtent(FVector(20));
}






void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	pistolComp->SetVisibility(false);
//	ChangeToNoEquipped();  // 스나이퍼건을 기본총으로
	anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());   // 애니메이션 블루프린트 저장
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;           // 시작은 걷기로
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      // 스나이퍼ui위젯생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);    // 크로스헤어위젯생성

	compBox -> OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::OnOverlap);

	HP = initialHP;
}

void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();   // 이동

	currentTime += DeltaTime;   
	
}




// 함수 내용 -------------------------------------------------------------------------------------------------------------------

void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); // 실행될 때 딱 한번 호출된다- 함수와 입력키와 연결(바인딩)해 준다

	// 2-1. 바인딩 
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);  
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::InputVertical);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::InputJump);   // 점프 
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);   // 발사 
	PlayerInputComponent->BindAction(TEXT("NoEquipped"), IE_Pressed, this, &ATPSPlayer::ChangeToNoEquipped);   // 그레나데건으로
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToPistol);   // 스나이퍼건으로
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);    // 스나이퍼 
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);    // 스나이퍼 
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATPSPlayer::InputRun); // 달리기
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATPSPlayer::InputRun);   // 걷기
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ATPSPlayer::InputCrouch); // 숙이기
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ATPSPlayer::InputCrouch);


//	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &ATPSPlayer::InputAttack);  // 공격
	PlayerInputComponent->BindAction(TEXT("Assasinate"), IE_Released, this, &ATPSPlayer::InputAssasinate);  // 암살
}

	
void ATPSPlayer::Turn(float value)   // 좌우회전
{
	AddControllerYawInput(value);  
}
	
void ATPSPlayer::LookUp(float value)    // 위아래 회전
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

void ATPSPlayer::InputJump()     // 점프
{
	Jump();     
}

void ATPSPlayer::Move()         // 무브 
{
	// 가는 방향으로 바라보기-
	direction = FTransform(GetControlRotation()).TransformVector(direction);   // 캐릭터 무브먼트 컴포넌트의 활용, walkspeed 필요없음
			 // 로테이션값을 받아와 디렉션을 바꾸겟다 = 이동방향을 컨트롤방향 기준으로 변환한다, 
				//이동  P = P0 + vt
				//FVector P0 = GetActorLocation();
				//FVector vt = direction * walkSpeed * DeltaTime;
				//FVector P = P0 + vt;
				//SetActorLocation(P);

	AddMovementInput(direction);
	direction = FVector::ZeroVector;   // 방향 제로 ????
}

void ATPSPlayer::InputRun()        // 달리기
{
	auto movement = GetCharacterMovement();
	if (bNoEquipped == true)
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

void ATPSPlayer::InputCrouch()
{
	auto movement = GetCharacterMovement();
	if (bNoEquipped == true)
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

void ATPSPlayer::InputFire()   // 발사 
{
	if (bNoEquipped)  // 비무장 상태면
	{              
		if (currentTime > attackDelayTime)
		{
			anim->isPlayerAttack = true;
			anim->PlayPunchAnim();
			currentTime = 0;
		}
	}
	else     // 스나이퍼건
	{
		if (pistolComp->IsVisible() == true)
		{
			if (currentBullet > 0)
			{
				anim->PlayAttackAnim();  // 공격 애니메이션 재생

				auto controller = GetWorld()->GetFirstPlayerController();   // 카메라 세이크 재생
				controller->PlayerCameraManager->StartCameraShake(cameraShake);

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

					UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);   // 발사 사운드 재생
					currentBullet--;
					UE_LOG(LogTemp, Warning, (TEXT("Current Bullet : %d")), currentBullet);
				}
				else
				{
					UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);   // 발사 사운드 재생
					currentBullet--;
					UE_LOG(LogTemp, Warning, (TEXT("Current Bullet : %d")), currentBullet);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, (TEXT("Lack Of Bullet")));
				return;
			}
		}
 	}
}

void ATPSPlayer::ChangeToNoEquipped()    // 5- 비무장 상태로
{
	_crosshairUI -> RemoveFromParent(); // 크로스헤어 UI 지우기
	pistolComp->SetVisibility(false);   // 권총 no
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  // 속도를 걷는 속도로
	anim->isGunEquipped = false;
	bNoEquipped = true;
}

void ATPSPlayer::ChangeToPistol()   // 권총으로
{
	if (bgetGun == true)  // 총을 주워야 함수 호출 가능
	{
		bNoEquipped = false;  // 비무장 상태가 아니고
		GetCharacterMovement()->MaxWalkSpeed=300;  // 걷는 속도를 300으로 설정
		_crosshairUI->AddToViewport();
		anim->isGunEquipped = true;  // 권총 든 애니메이션 실행
		pistolComp->SetVisibility(true);   //권총 yes
	}
	else return;
}

void ATPSPlayer::SniperAim()  // 6- 스나이퍼 입력 눌럿을 때 떼엇을 때
{
	if (bNoEquipped == true)    // 비무장 중일 때는 종료한다
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

// void ATPSPlayer::InputAttack()  // 공격하는 함수(마우스 좌클릭)
// {
// 	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Enemy::StaticClass());  // AIH_Enemy를 찾아서 actor 변수에 넣어둔다.
// 	if(actor == nullptr) return;
// 
// 	AIH_Enemy* enemy = Cast<AIH_Enemy>(actor);  // actor가 AIH_Enemy가 맞으면 enemy 변수에 저장한다.
// 	UEnemyFSM* enemyFSM = Cast<UEnemyFSM>(enemy);
// 
// 	
// 	if (currentTime > attackDelayTime)
// 	{
// 		enemy->fsm->OnDamageProcess();
// 		currentTime = 0;
// 	}
// }

void ATPSPlayer::InputAssasinate()  // 암살하는 함수(키보드 E)
{
	if (backEnemy != nullptr)
	{
		backEnemy->fsm->OnBackAttack();
		backEnemy = nullptr;
	}

//	AIH_Enemy* enemy = Cast<AIH_Enemy>(UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Enemy::StaticClass()));
//
//	if (isBack == true)
//	{
////		anim->PlayAssasinateAnim();
//		enemy->fsm->OnBackAttack();
//		isBack = false;
//	}
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