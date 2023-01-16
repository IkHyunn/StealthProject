

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




ATPSPlayer::ATPSPlayer()   // ������  --------------------------------------------------------------------------------
{
	PrimaryActorTick.bCanEverTick = true;

	// ���̷�Ż�޽� 
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Wise/Character/Player/Arissa.Arissa'"));
	if (TempMesh.Succeeded())  
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object); //GetMesh �Լ��� �̿��Ͽ� Mesh ������Ʈ�� ������ SetSkeletalMesh �Լ��� ȣ��.

		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));  //��ġ, ȸ���� 
	}

	// �������� 
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));  //���������Ʈ ���丮�Լ��� �̿��Ͽ� <  > �ν��Ͻ��� �����Ͽ� ������ ����
	springArmComp->SetupAttachment(RootComponent);      // ��Ʈ�� �ڽ�����
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));    // ��ġ
	springArmComp->TargetArmLength = 400;         // �� ����
	springArmComp->bUsePawnControlRotation = true;      //  ���������� ȸ���� ���ӵ� 

	// 1-ī�޶� 
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp")); 
	tpsCamComp->SetupAttachment(springArmComp);      // ���������� �ڽ�
	tpsCamComp->bUsePawnControlRotation = false;      // ī�޶�� ȸ���� ����
	bUseControllerRotationYaw = true;      //  �÷��̾�ȸ��- Ŭ������ ����Ʈ �������� �ǹ� 

	//��  �޽�
	pistolComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SniperGunComp"));  
	pistolComp->SetupAttachment(GetMesh(), TEXT("Pistol_Socket"));    // �޽��� �ڽ�, ���Ϻ��̱�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempPistolMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Pistols_A.Pistols_A'")); //  ������ �ε� //  TempSniperMesh ����ü(ConstructorHelpers)��������
	if(TempPistolMesh.Succeeded())  
	{
		pistolComp->SetSkeletalMesh(TempPistolMesh.Object);  
		pistolComp->SetRelativeLocation(FVector(0.218060, -9.718926, 1.827586));
		//(Pitch = 80.000000, Yaw = 449.999999, Roll = 270.000000)
		pistolComp->SetRelativeRotation(FRotator(80.000000, 449.999999, 270.000000));
	}

	// Ȱ �޽�
	bowComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("bowComp"));
	bowComp->SetupAttachment(GetMesh(), TEXT("LeftbowSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempbowMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/Bows/source/Bows_and_CrossBows_SketchFab_Ready_.Bows_and_CrossBows_SketchFab_Ready__Recurve Bow 2'"));
	if (tempbowMesh.Succeeded())
	{
		bowComp->SetSkeletalMesh(tempbowMesh.Object);
		bowComp->SetRelativeLocation(FVector(4.504357, -184.203023, -65.049796));
		bowComp->SetRelativeRotation(FRotator(19.987793, 93.703905, -50.103690));

	}

	// Į �޽�
	kalComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("kalComp"));
	kalComp->SetupAttachment(GetMesh(), TEXT("Kal_Socket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempkalMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Knife_A.Knife_A'"));
	if (tempkalMesh.Succeeded())
	{
		kalComp->SetSkeletalMesh(tempkalMesh.Object);
		kalComp->SetRelativeLocation(FVector(-2.605841,-8.806218,1.237086));
		kalComp->SetRelativeRotation(FRotator(-447.572166,-669.341707, 86.171881));
	}

	// ���� �ø���
	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	compBox->SetupAttachment(GetMesh(), TEXT("Hand_Socket"));
	compBox->SetBoxExtent(FVector(20));

	// 1.����Ŭ����
	ConstructorHelpers::FClassFinder<UUserWidget> _tempsniperUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_SniperUI.WBP_SniperUI_C'"));
	if (_tempsniperUI.Succeeded())
	{
		sniperUIFactory = _tempsniperUI.Class;
	}
	// 2.ũ�ν���� ����Ŭ���� ��������
	ConstructorHelpers::FClassFinder<UUserWidget> tempUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_Crosshair.WBP_Crosshair_C'"));
	if (tempUI.Succeeded())
	{
		crosshairUIFactory = tempUI.Class;
	}
	// 3.�Ѿ� �������Ʈ Ŭ���� ��������
	ConstructorHelpers::FClassFinder<ABullet> tempbulletFactory(TEXT("Blueprint'/Game/Wise/Blueprints/BP_Bullet.BP_Bullet_C'"));
	if (tempbulletFactory.Succeeded())
	{
		bulletFactory = tempbulletFactory.Class;
	}
	// 4.������ ��ƼŬ ������Ʈ ��������
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempEffectFactory(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect'"));
	if (tempEffectFactory.Succeeded())
	{
		bulletEffectFactory = tempEffectFactory.Object;
	}
	// 5.ī�޶� ����ũ �������Ʈ ��������
	ConstructorHelpers::FClassFinder<UCameraShakeBase> tempShakeBase(TEXT("Blueprint'/Game/Wise/Blueprints/BP_ShotCameraShake.BP_ShotCameraShake_C'"));
	if (tempShakeBase.Succeeded())
	{
		cameraShake = tempShakeBase.Class;
	}
	// 6.�� ���� ������Ʈ ��������
	ConstructorHelpers::FObjectFinder<USoundBase>tempSound(TEXT("SoundWave'/Game/StarterContent/Audio/Explosion01.Explosion01'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
}


void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// �ʱ⼳��
	pistolComp->SetVisibility(false);   // ��no  
	bowComp->SetVisibility(false);   // Ȱno
	kalComp->SetVisibility(false);   // Įno
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  //�ȱ��
	HP = initialHP;   // HP = 5
	anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());  // �÷��̾�ִ�cast 
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      //�������� ui ����
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);    // ũ�ν���� ui ���� 
	_crosshairUI->RemoveFromParent();  // ũ�ν���� �Ⱥ��̰�

	compBox -> OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::OnOverlap);  // ������
}



void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();   // �̵�
	currentTime += DeltaTime;  // �ð�����  �ʿ��Ѱ�??
}


void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); 
	//���ε� 
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);  
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::InputVertical);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::InputJump);   // ���� 
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);   // �߻� 
	PlayerInputComponent->BindAction(TEXT("Punch"), IE_Pressed, this, &ATPSPlayer::ChangeToPunch);   // 1�� �ָ�
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToPistol);   // ���� 2��
	PlayerInputComponent->BindAction(TEXT("Bow"), IE_Pressed, this, &ATPSPlayer::ChangeToBow); // Ȱ 3��
	PlayerInputComponent->BindAction(TEXT("Kal"), IE_Pressed, this, &ATPSPlayer::ChangeToKal); // Į 4��
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);    // ���� 
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);    
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATPSPlayer::InputRun); // �޸���
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATPSPlayer::InputRun);   // �ȱ�
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ATPSPlayer::InputCrouch); // ���̱�
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ATPSPlayer::InputCrouch);
	PlayerInputComponent->BindAction(TEXT("Assasinate"), IE_Released, this, &ATPSPlayer::InputAssasinate);  // �ϻ�
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
	direction = FTransform(GetControlRotation()).TransformVector(direction);   // ���� ����ٶ󺸱� 
	AddMovementInput(direction);
	direction = FVector::ZeroVector;   // direction �ʱ�ȭ
}

void ATPSPlayer::InputRun() // MaxWalkSpeed ����      
{
	auto movement = GetCharacterMovement();

	if(bCrouched == true) return;

	if (movement->MaxWalkSpeed > walkSpeed)
	{
		movement->MaxWalkSpeed = walkSpeed;   //�ȴ� �ӵ�
	}
	else
	{
		movement->MaxWalkSpeed = runSpeed;     //�޸��� �ӵ�
	}
}

void ATPSPlayer::InputCrouch()   // ��ũ����
{
	auto movement = GetCharacterMovement();
//	if (bPunch == true)   //�ָ��϶�  anim-> isCrouched �� true
//	{
		if (anim->isCrouched == false)    // ������ �� ���ָ��̸� ��ũ����
		{
			movement->MaxWalkSpeed = crouchSpeed;  // 200 ���� ������ �ȱ�
			anim->isCrouched = true;
			bCrouched = true;
		}
		else                             // ������ ��
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
	GetCharacterMovement()->DisableMovement();

	// ��ġ�̸�= 1���� �������� ���� ���� ���콺�� ��ġ�ϰ� �ٸ� ��ȣ�� ������ ������
	if (bPunch)
	{ 
		if (currentTime > attackDelayTime)
		{
			//anim->isPunch = true;
			compBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			anim->PlayPunchAnim();   // ��ġ�ִԽ����ϱ�

			currentTime = 0;
		}
	}
	
// 	2, 3, 4�� ���� ���� �ʱ�ȭ �ϸ� �Ǳ� ������ �ʿ����. InputFire�� ���콺 ��Ŭ��.
// 	if ((pistolComp->IsVisible() == true) || (bowComp->IsVisible() == true) || (kalComp->IsVisible() == true))
// 	{
// 		bPunch = false;
// 	}

	// Į�̸�
	if (kalComp->IsVisible() == true)
	{
		anim->PlayKalAimAnim();  //Į �ִ� ���� 
	}

	//�����̰� �Ѿ��� ������
	if ((pistolComp->IsVisible() == true) && (currentBullet > 0))    
	{ 
		anim->PlayAttackAnim();  //�� �ִ� ����
		LineTrace();  // ���� ����Ʈ���̽� 
	}

	// Ȱ�̸�
	if (bowComp->IsVisible() == true)
	{
		anim->PlayBowAimAnim();  //Ȱ �ִ� ���� 
		LineTrace();  // ���� ����Ʈ���̽� 
	}
}


void ATPSPlayer::ChangeToPunch()    //�ָ� -1�� 
{
	//���̱� ����
	pistolComp->SetVisibility(false);  
	kalComp->SetVisibility(false);   
	bowComp->SetVisibility(false);   
	_crosshairUI -> RemoveFromParent(); 

	// �ִԽ��࿩��
	anim->isPunch = true;  // �ָԾִ� = yes
	anim->isGunEquipped = false;    
	anim->isKal = false;  
	anim->isBow = false;  

	//�������� ����  
	bPunch = true;   // ���� ���콺(fire)  �������� ����
}



void ATPSPlayer::ChangeToPistol()   // �������� -2��
{
	if (bgetGun == true)  // ���� �ֿ��� �Լ� ȣ�� ����
	{
		//���̱� ����
		pistolComp->SetVisibility(true);   //���� yes
		_crosshairUI->AddToViewport();    // ũ�ν���� yes
		kalComp->SetVisibility(false);   
		bowComp->SetVisibility(false);   

		// �ִԽ��࿩��
		anim->isGunEquipped = true;  // ���� yes
		anim->isBow = false;  
		anim->isPunch = false;
		anim->isKal = false;  

		bPunch = false;
	}
}

void ATPSPlayer::ChangeToBow()   // Ȱ - 3��Ű 
{
	if (bgetbow == true)  
	{
		//���̱� ����
		bowComp->SetVisibility(true);   //Ȱ yes
		pistolComp->SetVisibility(false);  
		kalComp->SetVisibility(false);   

		// �ִԽ��࿩��
		anim->isBow = true;  // Ȱ yes
		anim->isGunEquipped = false; 
		anim->isKal = false;  
		anim->isPunch = false;

		bPunch = false;
	}
}

void ATPSPlayer::ChangeToKal()   // Į - 4��Ű 
{
	if (bgetKal == true)  
	{
		//���̱� ����
		kalComp->SetVisibility(true);  // Į yes
		_crosshairUI->RemoveFromParent();  // ũ�ν���� yes 
		pistolComp->SetVisibility(false);   
		bowComp->SetVisibility(false);   

		// �ִԽ��࿩��
		anim->isKal = true;  // Į yes
		anim->isBow = false;
		anim->isPunch = false;
		anim->isGunEquipped = false; 

		bPunch = false;
	}
}




void ATPSPlayer::SniperAim()  // 6- �������� �Է� ������ �� ������ ��
{
    // �Լ� �н��϶� : �ָ�, Į 
	//if (bPunch == true  ||  bgetKal == true)
	//{
	//	return;
	//}
	
	// �� �� ��  
	if (pistolComp->IsVisible() == true)
	{
		if (bSniperAim == false)  
		{
			_sniperUI->AddToViewport();   // �������� ����
			tpsCamComp->SetFieldOfView(45.0f);    //����
			_crosshairUI->RemoveFromParent();  // ũ�ν���� no
			bSniperAim = true;   //�ٲٰ�
		}
		else    
		{
			_sniperUI->RemoveFromParent();    // �������� �Ⱥ���
			tpsCamComp->SetFieldOfView(90.0f);   //�ܾƿ�
			_crosshairUI->AddToViewport();     // ũ�ν���� yes
			bSniperAim = false;     //�ٲٰ� 
		}
	}

	// Ȱ �� ��
	if (bowComp->IsVisible() == true)
	{
		if (bSniperAim == false)
		{
			_crosshairUI->AddToViewport();   // ũ�ν���� yes 
			tpsCamComp->SetFieldOfView(80.0f);
			bSniperAim = true;
		}
		else
		{
			_crosshairUI->RemoveFromParent();   // ũ�ν���� no 
			tpsCamComp->SetFieldOfView(90.0f);
			bSniperAim = false;
		}
	}
}


void ATPSPlayer::InputAssasinate()  // �ϻ��ϴ� �Լ�(Ű���� E)
{
	if (isBack == true)
	{
		FHitResult hitBack;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Enemy::StaticClass());
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitBack, GetActorLocation(), GetActorLocation()+GetActorForwardVector()*250, ECC_Visibility, params);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation()+GetActorForwardVector()*250,FColor::Red, false, 1.0f, 0, 1.0f);

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
	
				anim->PlayAssasinateAnim();

				backEnemy->fsm->OnBackAttack();
				isBack = false;
			}
		}
	}
}

void ATPSPlayer::fireEffect() 
{
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);   // �߻� ���� ���
	playerController = GetWorld()->GetFirstPlayerController();
	playerController->PlayerCameraManager->StartCameraShake(cameraShake);
}


void ATPSPlayer::LineTrace()  //��, Ȱ �߻� ����Ʈ���̽�
{
	FVector startPos = tpsCamComp->GetComponentLocation();  // ���� ��ġ
	FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;  // ������ġ
	FCollisionQueryParams params;     //  �浹�ɼ�
	params.AddIgnoredActor(this);      // �ڱ��ڽ�����

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);  

	if (bHit)
	{
		if (pistolComp->IsVisible() == true)
		{
			FTransform bulletTrans;   // �Ѿ����� ȿ�� 
			bulletTrans.SetLocation(hitInfo.ImpactPoint);     // �ε��� ��ġ �Ҵ�  ???
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);  // ����Ʈ����
			fireEffect();  // ���� & ī�޶���ũ
			currentBullet--;  //�Ѿ˰���
		}
	
		auto hitComp = hitInfo.GetComponent();  
		if (hitComp && hitComp->IsSimulatingPhysics())   // ����������  
		{
			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;  
			hitComp->AddForce(force);    
		}

		auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));   //  FSM �� �ε������� ��
		if (enemy)
		{
			auto enemyFSM = Cast<UEnemyFSM>(enemy);     // �ֳʹ�fsm ĳ��Ʈ
			enemyFSM->OnDamageProcess();
		}
	}
}




void ATPSPlayer::OnHitEvent()  // �ǰ� �̺�Ʈ
{
	GetCharacterMovement()->DisableMovement();
	UE_LOG(LogTemp, Warning, TEXT("Player Damaged!"));
	HP--;
	UE_LOG(LogTemp, Warning, TEXT("Player HP : %d"), HP);

		if (HP <= 0)  // �������·� 
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Dead!"));
			this->PlayAnimMontage(anim->DamageDieMontage, 1.0f, FName(TEXT("Die")));  // ���� ��Ÿ�� ���
			//anim->PlayDamageAnim(TEXT("Die"));
			if (anim->isDieDone == true)   // Die ������ ������ 
			{
				OnGameOver();  // ���� ���� �Լ� ȣ��
			}
		}
		else  // �ǰݻ��·�
		{
			int32 index = FMath::RandRange(0, 1);   // �ǰ� ��Ÿ�� �����ϰ� ���
			FString sectionName = FString::Printf(TEXT("Damage%d"), index);
			this->PlayAnimMontage(anim->DamageDieMontage, 1.0f, FName(*sectionName));
			//anim->PlayDamageAnim(FName(*sectionName));  // �ǰ� ��Ÿ�ֽ����ϱ�
		}
}

void ATPSPlayer::OnGameOver()
{
	AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();  // currMode�� ���� ���Ӹ���� AStealthProjectGameModeBase�� �����´�.

	currMode->ShowGameOverUI();  // ���Ӹ�忡�� ���� ShowGameOverUI �Լ��� ȣ���Ѵ�.
	UGameplayStatics::SetGamePaused(GetWorld(), true);  // ������ �����.
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);  // ���콺 Ŀ���� ȭ�鿡 ����.
}

void ATPSPlayer::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		AIH_Enemy* enemy = Cast<AIH_Enemy>(OtherActor);     // �ֳʹ� ĳ��Ʈ�Ѵ�

		if (enemy != nullptr)
		{
			if (enemy->fsm->HP > 0)
			{
				if (isOnAttack == true)
				{
					enemy->fsm->OnDamageProcess();
				}
			}
		}
	}
}