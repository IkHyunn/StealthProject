

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
#include "CrosshairUI.h"




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
		kalComp->SetRelativeLocation(FVector(-2.605841,-8.806218,1.237086));
		kalComp->SetRelativeRotation(FRotator(-447.572166,-669.341707, 86.171881));
	}

	// 오른손 Collision
	righthandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHand"));
	righthandBox->SetupAttachment(GetMesh(), TEXT("Hand_Socket"));
	righthandBox->SetBoxExtent(FVector(15));
	righthandBox->SetCollisionProfileName(TEXT("Hand"));

	// 왼손 Collision
	lefthandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHand"));
	lefthandBox->SetupAttachment(GetMesh(), TEXT("LeftHand_Socket"));
	lefthandBox->SetBoxExtent(FVector(15));
	lefthandBox->SetCollisionProfileName(TEXT("Hand"));

	// 칼 Collision
	knifeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("knifeBox"));
	knifeBox->SetupAttachment(GetMesh(), TEXT("Kal_Socket"));
	knifeBox->SetBoxExtent(FVector(15));
	knifeBox->SetCollisionProfileName(TEXT("Hand"));

	// 1.위젯클래스
	ConstructorHelpers::FClassFinder<UUserWidget> _tempsniperUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_SniperUI.WBP_SniperUI_C'"));
	if (_tempsniperUI.Succeeded())
	{
		sniperUIFactory = _tempsniperUI.Class;
	}
	// 2. crosshairUI 클래스 가져오기
	ConstructorHelpers::FClassFinder<UCrosshairUI>tempcrosshair(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_CrossHairUI.WG_CrossHairUI_C'"));
	if (tempcrosshair.Succeeded())
	{
		crosshairFactory = tempcrosshair.Class;
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
	// 5.카메라 쉐이크 블루프린트 가져오기
	ConstructorHelpers::FClassFinder<UCameraShakeBase> tempShakeBase(TEXT("Blueprint'/Game/Wise/Blueprints/BP_ShotCameraShake.BP_ShotCameraShake_C'"));
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
}


void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// 초기설정
	bowComp->SetVisibility(false);   // 활no
	kalComp->SetVisibility(false);   // 칼no
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  //걷기로
	HP = initialHP;   // HP = 5
	anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());  // 플레이어애님cast 
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      //스나이퍼 ui 생성
	crosshairUI = CreateWidget<UCrosshairUI>(GetWorld(), crosshairFactory);

	righthandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	righthandBox -> OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::HandOverlap);

	lefthandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	lefthandBox->OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::LeftHandOverlap);

	knifeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	knifeBox->OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::KnifeOverlap);
}



void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();   // 이동
	currentTime += DeltaTime;  // 시간누적  필요한가??
	
	if (bSniperAim == true)
	{
		tpsCamComp->SetFieldOfView(zoomIn);    //줌인
		zoomIn -= DeltaTime * 300;

		if (zoomIn <= 45)
		{
			zoomIn = 45;
		}
	}
	else
	{
		tpsCamComp->SetFieldOfView(zoomIn);
		zoomIn += DeltaTime * 300;

		if (zoomIn >= 90)
		{
			zoomIn = 90;
		}
	}
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
	PlayerInputComponent->BindAction(TEXT("Bow"), IE_Pressed, this, &ATPSPlayer::ChangeToBow); // 활 2번
	PlayerInputComponent->BindAction(TEXT("Kal"), IE_Pressed, this, &ATPSPlayer::ChangeToKal); // 칼 3번
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

	if(bCrouched == true) return;

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
//	if (bPunch == true)   //주먹일때  anim-> isCrouched 는 true
//	{
		if (anim->isCrouched == false)    // 눌럿을 때 맨주먹이며 웅크리면
		{
			movement->MaxWalkSpeed = crouchSpeed;  // 200 으로 느리게 걷기
			anim->isCrouched = true;
			bCrouched = true;
		}
		else                             // 떼엇을 때
		{
			movement->MaxWalkSpeed = walkSpeed;
			anim->isCrouched = false;
			bCrouched = false;
		}
//	}
//	else return;
}

void ATPSPlayer::InputFire()  
{

	// 펀치이면= 1번이 눌러졌을 때만 왼쪽 마우스로 펀치하고 다른 번호를 눌러야 나간다
	if (bPunch)
	{ 
		if (isAttacking == false)
		{
			isAttacking = true;
			GetCharacterMovement()->DisableMovement();
		
			switch (attackCount)
			{
				case 0:
				attackCount = 1;
				righthandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayPunchAnim();
				break;
				case 1:
				attackCount = 0;
				lefthandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayHookAnim();
				break;
			}
		}	
		else
		{
			saveAttack = true;
		}
	}

	// 칼이면
	if (kalComp->IsVisible() == true)
	{
		if (isAttacking == false)
		{
			isAttacking = true;
			GetCharacterMovement()->DisableMovement();

			switch (attackCount)
			{
			case 0:
				attackCount = 1;
				knifeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayKalAnim();
				break;
			case 1:
				attackCount = 2;
				knifeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayKal2Anim();
				break;
			case 2:
				attackCount = 0;
				knifeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayKal3Anim();
				break;
			}
		}
		else
		{
			saveAttack = true;
		}
	}

	// 활이면
	if (bowComp->IsVisible() == true && currentBullet > 0)
	{
		LineTrace();
		GetCharacterMovement()->DisableMovement();
		anim->PlayBowAimAnim();  //활 애님 실행 
	}
}

void ATPSPlayer::ComboAttackSave()
{
	if (bPunch == true)
	{
		if (saveAttack == true)
		{
			saveAttack = false;

			switch (attackCount)
			{
			case 0:
				attackCount = 1;
				righthandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayPunchAnim();
				break;
			case 1:
				attackCount = 0;
				lefthandBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayHookAnim();
				break;
			}
		}
	}
	
	if (kalComp->IsVisible() == true)
	{
		if (saveAttack == true)
		{
			saveAttack = false;

			switch (attackCount)
			{
			case 0:
				attackCount = 1;
				knifeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayKalAnim();
				break;
			case 1:
				attackCount = 2;
				knifeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayKal2Anim();
				break;
			case 2:
				attackCount = 0;
				knifeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				anim->PlayKal3Anim();
				break;
			}
		}
	}
}

void ATPSPlayer::ComboReset()
{
	attackCount = 0;
	isAttacking = false;
	saveAttack = false;
}

void ATPSPlayer::ChangeToPunch()    //주먹 -1번 
{
	//보이기 여부
	kalComp->SetVisibility(false);   
	bowComp->SetVisibility(false);   
	crosshairUI -> RemoveFromParent(); 

	// 애님실행여부
	anim->isPunch = true;  // 주먹애님 = yes
	anim->isGunEquipped = false;    
	anim->isKal = false;  
	anim->isBow = false;  

	//조건위한 변수  
	bPunch = true;   // 왼쪽 마우스(fire)  선별위한 변수
}

void ATPSPlayer::ChangeToBow()   // 활 - 3번키 
{
	if (bgetbow == true)  
	{
		//보이기 여부
		bowComp->SetVisibility(true);   //활 yes
		kalComp->SetVisibility(false);
		crosshairUI->AddToViewport();

		// 애님실행여부
		anim->isBow = true;  // 활 yes
		anim->isGunEquipped = false; 
		anim->isKal = false;  
		anim->isPunch = false;

		bPunch = false;
	}
}

void ATPSPlayer::ChangeToKal()   // 칼 - 4번키 
{
	if (bgetKal == true)  
	{
		//보이기 여부
		kalComp->SetVisibility(true);  // 칼 yes
		crosshairUI->RemoveFromParent();  // 크로스헤어 yes 
		bowComp->SetVisibility(false);   

		// 애님실행여부
		anim->isKal = true;  // 칼 yes
		anim->isBow = false;
		anim->isPunch = false;
		anim->isGunEquipped = false; 

		bPunch = false;
	}
}

void ATPSPlayer::SniperAim()  // 6- 스나이퍼 입력 눌럿을 때 떼엇을 때
{
     // 함수 패스일때 : 주먹, 칼 
 	if (bPunch == true  ||  kalComp->IsVisible() == true)
 	{
 		return;
 	}

	if (bCrouched == false)
	{
		if (bowComp->IsVisible() == true)  // 활일 때
		{
			if (bSniperAim == false)
			{
				anim->isBowAiming = true;
				crosshairUI->ZoomIn();
				bSniperAim = true;
			}
			else
			{
				anim->isBowAiming = false;
				crosshairUI->ZoomOut();
				bSniperAim = false;
			}
		}
	}
}


void ATPSPlayer::InputAssasinate()  // 암살하는 함수(키보드 E)
{
	if (isBack == true)
	{
		FHitResult hitBack;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitBack, GetActorLocation(), GetActorLocation()+GetActorForwardVector()*250, ECC_Visibility, params);
		//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation()+GetActorForwardVector()*250,FColor::Red, false, 1.0f, 0, 1.0f);

		if (bHit)
		{
			if (hitBack.GetActor()->GetName().Contains(TEXT("Enemy")))
			{
				
				backEnemy = Cast<AIH_Enemy>(hitBack.GetActor());
				FVector playerPos = backEnemy->GetActorLocation() + backEnemy->GetActorForwardVector() * -180.0;

				playerController = GetWorld()->GetFirstPlayerController();
				this->DisableInput(playerController);

				SetActorLocation(playerPos);
				SetActorRotation(backEnemy->GetActorRotation());

				bUseControllerRotationYaw = false;
				anim->PlayAssasinateAnim();

				backEnemy->fsm->OnBackAttack();
				isBack = false;
			}
		}
	}
}

void ATPSPlayer::fireEffect() 
{
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);   // 발사 사운드 재생
	playerController = GetWorld()->GetFirstPlayerController();
	playerController->PlayerCameraManager->StartCameraShake(cameraShake);
}

void ATPSPlayer::LineTrace()  //총, 활 발사 라인트레이스
{
	FVector startPos = tpsCamComp->GetComponentLocation();  // 시작 위치
	FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;  // 종료위치
	FCollisionQueryParams params;     //  충돌옵션
	params.AddIgnoredActor(this);      // 자기자신제외

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);  

	if (bHit)
	{
		currentBullet--;  //총알 감소
	
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
			enemyFSM->OnDamageProcess(2);
		}
	}
}




void ATPSPlayer::OnHitEvent()  // 피격 이벤트
{
	GetCharacterMovement()->DisableMovement();
	ComboReset();
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

void ATPSPlayer::HandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		AIH_Enemy* enemy = Cast<AIH_Enemy>(OtherActor);     // 애너미 캐스트한다

		if (enemy != nullptr)
		{
			if (enemy->fsm->currHP > 0)
			{
				enemy->fsm->OnDamageProcess(1);
			}
		}
	}
}

void ATPSPlayer::LeftHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		AIH_Enemy* enemy = Cast<AIH_Enemy>(OtherActor);
		if (enemy != nullptr)
		{
			if (enemy->fsm->currHP > 0)
			{
				enemy->fsm->OnDamageProcess(1);
			}
		}
	}
}

void ATPSPlayer::KnifeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		AIH_Enemy* enemy = Cast<AIH_Enemy>(OtherActor);     // 애너미 캐스트한다

		if (enemy != nullptr)
		{
			if (enemy->fsm->currHP > 0)
			{
				enemy->fsm->OnDamageProcess(2);
			}
		}
	}
}