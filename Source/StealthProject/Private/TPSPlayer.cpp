

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
#include "ArrowCountUI.h"
#include "PlayerHP.h"
#include "IH_CountUI.h"
#include "PlusCountUI.h"
#include "EnemyHP.h"
#include "IH_Bullet.h"
#include "IH_Knife.h"
#include "Kal.h"
#include "IH_HPItem.h"




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

	// ������ Collision
	righthandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHand"));
	righthandBox->SetupAttachment(GetMesh(), TEXT("Hand_Socket"));
	righthandBox->SetBoxExtent(FVector(15));
	righthandBox->SetCollisionProfileName(TEXT("Hand"));

	// �޼� Collision
	lefthandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHand"));
	lefthandBox->SetupAttachment(GetMesh(), TEXT("LeftHand_Socket"));
	lefthandBox->SetBoxExtent(FVector(15));
	lefthandBox->SetCollisionProfileName(TEXT("Hand"));

	// Į Collision
	knifeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("knifeBox"));
	knifeBox->SetupAttachment(GetMesh(), TEXT("Kal_Socket"));
	knifeBox->SetBoxExtent(FVector(15));
	knifeBox->SetCollisionProfileName(TEXT("Hand"));

	// 1.����Ŭ����
	ConstructorHelpers::FClassFinder<UUserWidget> _tempsniperUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_SniperUI.WBP_SniperUI_C'"));
	if (_tempsniperUI.Succeeded())
	{
		sniperUIFactory = _tempsniperUI.Class;
	}
	// 2. crosshairUI Ŭ���� ��������
	ConstructorHelpers::FClassFinder<UCrosshairUI>tempcrosshair(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_CrossHairUI.WG_CrossHairUI_C'"));
	if (tempcrosshair.Succeeded())
	{
		crosshairFactory = tempcrosshair.Class;
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
	// 7. ȭ�� ī��Ʈ ���� �������Ʈ ��������
	ConstructorHelpers::FClassFinder<UArrowCountUI>tempArrow(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_ArrowCountUI.WG_ArrowCountUI_C'"));
	if (tempArrow.Succeeded())
	{
		arrowcountFactory = tempArrow.Class;
	}
	// 8. HP ���� �������Ʈ ��������
	ConstructorHelpers::FClassFinder<UPlayerHP>temphpbar(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_PlayerHP.WG_PlayerHP_C'"));
	if (temphpbar.Succeeded())
	{
		playerHPFactory = temphpbar.Class;
	}
	// 9. �� ī��Ʈ ���� �������Ʈ ��������
	ConstructorHelpers::FClassFinder<UPlusCountUI>tempplus(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_PlusCountUI.WG_PlusCountUI_C'"));
	if (tempplus.Succeeded())
	{
		pluscountFactory = tempplus.Class;
	}

	// ���� ��������
	ConstructorHelpers::FObjectFinder<USoundBase>temppickknife(TEXT("SoundWave'/Game/Wise/Resources/Sound/Foley/FOL_Pick_Dagger.FOL_Pick_Dagger'"));
	if (temppickknife.Succeeded())
	{
		pickKnifeSound = temppickknife.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase>temppickbow(TEXT("SoundWave'/Game/Wise/Resources/Sound/Foley/FOL_Pick_Bow.FOL_Pick_Bow'"));
	if (temppickbow.Succeeded())
	{
		pickBowSound = temppickbow.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase>temppickarrow(TEXT("SoundWave'/Game/Wise/Resources/Sound/Foley/FOL_Pick_Arrow.FOL_Pick_Arrow'"));
	if (temppickarrow.Succeeded())
	{
		pickArrowSound = temppickarrow.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase>tempbowloading(TEXT("SoundWave'/Game/Wise/Resources/Sound/Foley/FOL_BowLoading.FOL_BowLoading'"));
	if (tempbowloading.Succeeded())
	{
		bowLoadingSound = tempbowloading.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase>temppunch(TEXT("SoundCue'/Game/Wise/Resources/Sound/FX/sc_FX_PunchDamaged.sc_FX_PunchDamaged'"));
	if (temppunch.Succeeded())
	{
		punchDamagedSound = temppunch.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase>tempsword(TEXT("SoundCue'/Game/Wise/Resources/Sound/FX/sc_FX_SwordDamaged.sc_FX_SwordDamaged'"));
	if (tempsword.Succeeded())
	{
		swordDamagedSound = tempsword.Object;
	}
}


void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// �ʱ⼳��
	bowComp->SetVisibility(false);   // Ȱno
	kalComp->SetVisibility(false);   // Įno
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  //�ȱ��
	HP = maxHP;   // HP = 5
	anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());  // �÷��̾�ִ�cast 
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      //�������� ui ����
	crosshairUI = CreateWidget<UCrosshairUI>(GetWorld(), crosshairFactory);
	arrowcountUI = CreateWidget<UArrowCountUI>(GetWorld(), arrowcountFactory);
	playerHPUI = CreateWidget<UPlayerHP>(GetWorld(), playerHPFactory);
	playerHPUI->AddToViewport();
	plusCountUI = CreateWidget<UPlusCountUI>(GetWorld(), pluscountFactory);

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
	Move();   // �̵�
	currentTime += DeltaTime;  // �ð�����  �ʿ��Ѱ�??
	
	if (bSniperAim == true)
	{
		tpsCamComp->SetFieldOfView(zoomIn);    //����
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
	//���ε� 
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);  
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::InputVertical);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::InputJump);   // ���� 
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);   // �߻� 
	PlayerInputComponent->BindAction(TEXT("Punch"), IE_Pressed, this, &ATPSPlayer::ChangeToPunch);   // 1�� �ָ�
	PlayerInputComponent->BindAction(TEXT("Bow"), IE_Pressed, this, &ATPSPlayer::ChangeToBow); // Ȱ 2��
	PlayerInputComponent->BindAction(TEXT("Kal"), IE_Pressed, this, &ATPSPlayer::ChangeToKal); // Į 3��
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);    // ���� 
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);    
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATPSPlayer::InputRun); // �޸���
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATPSPlayer::InputRun);   // �ȱ�
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ATPSPlayer::InputCrouch); // ���̱�
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ATPSPlayer::InputCrouch);
	PlayerInputComponent->BindAction(TEXT("Assasinate"), IE_Released, this, &ATPSPlayer::InputAssasinate);  // �ϻ�
	PlayerInputComponent->BindAction(TEXT("MissionTab"), IE_Pressed, this, &ATPSPlayer::InputMission);
	PlayerInputComponent->BindAction(TEXT("MissionTab"), IE_Released, this, &ATPSPlayer::InputMission);
	PlayerInputComponent->BindAction(TEXT("PickUp"), IE_Pressed, this, &ATPSPlayer::InputPickUp);
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

	if (anim->isCrouched == false)
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
}

void ATPSPlayer::InputFire()  
{

	// ��ġ�̸�= 1���� �������� ���� ���� ���콺�� ��ġ�ϰ� �ٸ� ��ȣ�� ������ ������
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

	// Į�̸�
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

	// Ȱ�̸�
	if (bowComp->IsVisible() == true && currentArrow > 0)
	{
		LineTrace();
		GetCharacterMovement()->DisableMovement();
		anim->PlayBowAimAnim();  //Ȱ �ִ� ���� 
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

void ATPSPlayer::ChangeToPunch()    //�ָ� -1�� 
{
	//���̱� ����
	kalComp->SetVisibility(false);   
	bowComp->SetVisibility(false);   
	crosshairUI -> RemoveFromParent(); 
	arrowcountUI->RemoveFromParent();

	// �ִԽ��࿩��
	anim->isPunch = true;  // �ָԾִ� = yes
	anim->isGunEquipped = false;    
	anim->isKal = false;  
	anim->isBow = false;  

	//�������� ����  
	bPunch = true;   // ���� ���콺(fire)  �������� ����
}

void ATPSPlayer::ChangeToBow()   // Ȱ - 3��Ű 
{
	if (bgetbow == true)  
	{
		//���̱� ����
		bowComp->SetVisibility(true);   //Ȱ yes
		kalComp->SetVisibility(false);
		crosshairUI->AddToViewport();
		arrowcountUI->AddToViewport();

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
		crosshairUI->RemoveFromParent();  // ũ�ν���� yes 
		bowComp->SetVisibility(false); 
		arrowcountUI->RemoveFromParent();

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
 	if (bPunch == true  ||  kalComp->IsVisible() == true)
 	{
 		return;
 	}

	if (bCrouched == false)
	{
		if (bowComp->IsVisible() == true)  // Ȱ�� ��
		{
			if (bSniperAim == false)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), bowLoadingSound, GetActorLocation());
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


void ATPSPlayer::InputAssasinate()  // �ϻ��ϴ� �Լ�(Ű���� E)
{
	if (isBack == true)
	{
		anim->isCrouched=false;
		bCrouched=false;
		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

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

void ATPSPlayer::InputMission()
{
	AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();
	
	if (currMode->countUI->IsInViewport() == false)
	{
		currMode->countUI->AddToViewport();
	}
	else
	{
		currMode->countUI->RemoveFromParent();
	}
}


void ATPSPlayer::InputPickUp()
{
	if (pickBow != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), pickBowSound, GetActorLocation());
		bgetbow = true;      //  Ȱ���� = true, true�� ���� 3�� Ű�� ���� �� ����.
		anim->isBow = true;  //   Ȱ �ִ� = true
		ChangeToBow();   //  Ȱ �Լ�ȣ��
	 
		anim->isGunEquipped = false;  // �Ѿִ� no
		pickBow->Destroy();
		pickBow = nullptr;
	}

	if (pickArrow != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), pickArrowSound, GetActorLocation());
		currentArrow+= pickArrow->plusBullet;
		arrowcountUI->UpdateCurrentArrow(currentArrow);
		pickArrow->Destroy();
		pickArrow = nullptr;
	}

	if (pickHPItem != nullptr)
	{
		HP += 3;
 
 		if (HP >= 5)
 		{
 			HP=maxHP;
 			playerHPUI->UpdatePlayerHP(maxHP, maxHP);
 		}
 		else
		{
			playerHPUI->UpdatePlayerHP(HP, maxHP);
		}

		pickHPItem->Destroy();
		pickHPItem = nullptr;
	}

	if (pickKnife != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), pickKnifeSound, GetActorLocation());
		bgetKal = true;      //  Į���� = true, true�� ���� 3�� Ű�� ���� �� ����.
	 	ChangeToKal();   //  Į �Լ�ȣ��
	 	anim->isKal = true;  //   Į �ִ� = true
	 	anim->isGunEquipped = false;  // �Ѿִ� no
	
	 	pickKnife->Destroy();
		pickKnife = nullptr;
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

	currentArrow--;  //�Ѿ� ����
	arrowcountUI->UpdateCurrentArrow(currentArrow);

	if (bHit)
	{
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
			enemyFSM->OnDamageProcess(2);
		}
	}
}




void ATPSPlayer::OnHitEvent()  // �ǰ� �̺�Ʈ
{
	GetCharacterMovement()->DisableMovement();
	ComboReset();
	ReceiveDamage(1);

	if (HP <= 0)  // �������·� 
	{
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

void ATPSPlayer::ReceiveDamage(float damage)
{
	HP -= damage;

	playerHPUI->UpdatePlayerHP(HP, maxHP);
}


void ATPSPlayer::OnGameOver()
{
	AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();  // currMode�� ���� ���Ӹ���� AStealthProjectGameModeBase�� �����´�.

	currMode->ShowGameOverUI();  // ���Ӹ�忡�� ���� ShowGameOverUI �Լ��� ȣ���Ѵ�.
	UGameplayStatics::SetGamePaused(GetWorld(), true);  // ������ �����.
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);  // ���콺 Ŀ���� ȭ�鿡 ����.
}

void ATPSPlayer::HandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		AIH_Enemy* enemy = Cast<AIH_Enemy>(OtherActor);     // �ֳʹ� ĳ��Ʈ�Ѵ�

		if (enemy != nullptr)
		{
			if (enemy->fsm->currHP > 0)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), punchDamagedSound, GetActorLocation());
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
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), punchDamagedSound, GetActorLocation());
				enemy->fsm->OnDamageProcess(1);
			}
		}
	}
}

void ATPSPlayer::KnifeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		AIH_Enemy* enemy = Cast<AIH_Enemy>(OtherActor);     // �ֳʹ� ĳ��Ʈ�Ѵ�

		if (enemy != nullptr)
		{
			if (enemy->fsm->currHP > 0)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), swordDamagedSound, GetActorLocation());
				enemy->fsm->OnDamageProcess(2);
			}
		}
	}
}