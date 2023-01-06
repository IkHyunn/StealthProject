// ����Ű alt + o




#include "TPSPlayer.h"   // �÷��̾�
#include <GameFramework/SpringArmComponent.h>   //  �������� 
#include <Camera/CameraComponent.h>        // ī�޶� 
#include "Bullet.h"     // �Ѿ�
#include <UObject/ConstructorHelpers.h>  // 5- ��ź�� & �������۰�
#include <Blueprint/UserWidget.h>    // 6- �������� UI
#include <Kismet/GameplayStatics.h>   // 6- 3  �Ѿ�����ȿ�� ������
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>   // 10- ���϶���?
#include "IH_Enemy.h"
#include "EnemyFSM.h"
#include "../StealthProjectGameModeBase.h"
#include <GameFramework/CharacterMovementComponent.h>    // 10-
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
		pistolComp->SetRelativeLocation(FVector(-8.385662, 3.357624, -2.620080));   
		pistolComp->SetRelativeRotation(FRotator(0, 90, 0));     
	}
	// Į �޽�
	knifeComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("KnifeComp"));
	knifeComp->SetupAttachment(GetMesh(), TEXT("Knife_Socket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempKnifeMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Knife_A.Knife_A'"));
	if (tempKnifeMesh.Succeeded())
	{
		knifeComp->SetSkeletalMesh(tempKnifeMesh.Object);
		knifeComp->SetRelativeLocation(FVector(-8.385662, 3.357624, -2.620080));
		knifeComp->SetRelativeRotation(FRotator(0, 90, 0));
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
	//UPROPERTY(EditDefaultsOnly, Category = CameraMotion)     //ī�޶���ũ �������Ʈ�� ������ ����
	//	TSubclassOf<class UCameraShakeBase> cameraShake;
	// 5.ī�޶� ����ũ �������Ʈ ��������
	ConstructorHelpers::FClassFinder<UCameraShakeBase> tempShakeBase(TEXT("Blueprint'/Game/Wise/Blueprints/BP_CamerShake.BP_CamerShake_C'"));
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
	
	JumpMaxCount = 2;   // ���� ����
}


void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// �ʱ⼳��
	pistolComp->SetVisibility(false);   // ��no  
	knifeComp->SetVisibility(false);   // Įno
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  //�ȱ��
	HP = initialHP;   // hp = 0

	anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());   // ó���� �÷��̾��� anim ĳ��Ʈ
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      // ��������ui��������
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);    // ũ�ν������������

	compBox -> OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::OnOverlap); // �����ø���?
}



void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();   // �̵�
	currentTime += DeltaTime;  // �ð����� 

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
	PlayerInputComponent->BindAction(TEXT("NoEquipped"), IE_Pressed, this, &ATPSPlayer::ChangeToNoEquipped);   // �ָ�
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToPistol);   // ����
	PlayerInputComponent->BindAction(TEXT("Knife"), IE_Pressed, this, &ATPSPlayer::ChangeToKnife); // Į
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);    // ���� 
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);    // 
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
	direction = FVector::ZeroVector;   
}

void ATPSPlayer::InputRun() // MaxWalkSpeed ����      
{
	auto movement = GetCharacterMovement();
	if (bNoEquipped == true)    // ���ָ��̸� �޸��� 
	{
		if (movement->MaxWalkSpeed > walkSpeed)
		{
			movement->MaxWalkSpeed = walkSpeed;   //�ȴ� �ӵ�
		}
		else
		{
			movement->MaxWalkSpeed = runSpeed;     //�޸��� �ӵ�
		}
	}
	else return;
}

void ATPSPlayer::InputCrouch()   // ��ũ����
{
	auto movement = GetCharacterMovement();
	if (bNoEquipped == true)   //�ָ�
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
	if (bNoEquipped)  // �ָ����̸�
	{              
		if (currentTime > attackDelayTime)  // �����ð��� ������ 
		{
			anim->isPlayerAttack = true;  // ���þִ� True
			anim->PlayPunchAnim();   // ��ġ�ִԽ���
			currentTime = 0; // �ð��ʱ�ȭ
			return;
		}
	}
	if ((pistolComp->IsVisible() == true) && (currentBullet > 0))   //�����̰� �Ѿ��� ������ 
	{ 
		anim->PlayAttackAnim();  //�� �ִԽ���
		LineTrace();  // ���� ����Ʈ���̽� 
	}
}

void ATPSPlayer::ChangeToNoEquipped()    //�ָ� -1�� 
{
	//�Ⱥ��̱�
	_crosshairUI -> RemoveFromParent(); // ũ�ν���� UI �Ⱥ��̱�
	pistolComp->SetVisibility(false);   // ���� �Ⱥ��̱�
	anim->isGunEquipped = false;       // ���Ѿִ� = no
	knifeComp->SetVisibility(false);   // Į �Ⱥ��̱�
	                                   // Į�ִ� = no
	// ���̱�
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  // �ӵ��� �ȴ� �ӵ���
	bNoEquipped = true;  // �ָԾִ� = yes
}

void ATPSPlayer::ChangeToPistol()   // �������� -2��
{
	if (bgetGun == true)  // ���� �ֿ��� �Լ� ȣ�� ����
	{
		bNoEquipped = false;  // 
		GetCharacterMovement()->MaxWalkSpeed=300;  // �ȴ� �ӵ�
		_crosshairUI->AddToViewport();    // ũ�ν���� ���̱�
		anim->isGunEquipped = true;  // ���� �� �ִϸ��̼� ����
		pistolComp->SetVisibility(true);   //���� yes
		knifeComp->SetVisibility(false);   //Į no
	}
}

void ATPSPlayer::ChangeToKnife()   // Į - 3��Ű 
{
	if (bgetKnife == true)  // Į ���� ���Ŀ� ���ķ� 3��Ű ���� �� �Լ� ȣ�� ����
	{
		bNoEquipped = false;  // ���ָ� no
		pistolComp->SetVisibility(false);   //���� no
		knifeComp->SetVisibility(true);   //Į yes
		anim->isGunEquipped = false;
		_crosshairUI->RemoveFromParent(); // ũ�ν���� UI �����
		                   // ��ġ�ִԽ��� // Į �ִϸ��̼� ����   !!!!
	}
}

void ATPSPlayer::SniperAim()  // 6- �������� �Է� ������ �� ������ ��
{
	if (bNoEquipped == true)   
	{
		return;
	}
	// 
	if (bSniperAim == false)  
	{
		bSniperAim = true;    // �ʱⰪ�� false �����Ƿ� true �� �ٲٰ�
		_sniperUI->AddToViewport();     
		tpsCamComp->SetFieldOfView(45.0f);    
		_crosshairUI->RemoveFromParent();  //
	}
	//  
	else
	{
		bSniperAim = false;     // 
		_sniperUI->RemoveFromParent();    // ui �Ⱥ��̰�
		tpsCamComp->SetFieldOfView(90.0f);   //ī�޶� �þ߰� ������� 90
		_crosshairUI->AddToViewport();     //6- �Ϲ����� ui ���
	}
}


void ATPSPlayer::InputAssasinate()  // �ϻ��ϴ� �Լ�(Ű���� E)
{
	if (backEnemy != nullptr)
	{
		backEnemy->fsm->OnBackAttack();
		backEnemy = nullptr;
	}
}

void ATPSPlayer::fireEffect() 
{
	// �߻� ���� ���
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);
	// ī�޶� ����ũ ���
	auto controller = GetWorld()->GetFirstPlayerController();  
	controller->PlayerCameraManager->StartCameraShake(cameraShake);
}

void ATPSPlayer::LineTrace()  //�� �߻� ����Ʈ���̽�
{
	FVector startPos = tpsCamComp->GetComponentLocation();  // ���� ��ġ
	FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;  // ������ġ
	// FHitResult hitInfo;       //�浹����  - ����������??
	FCollisionQueryParams params;     //  �浹�ɼ�
	params.AddIgnoredActor(this);      // �ڱ��ڽ�����

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);  
	
	FTransform bulletTrans;   // �Ѿ����� ȿ�� 
	bulletTrans.SetLocation(hitInfo.ImpactPoint);     // �ε��� ��ġ �Ҵ�  ???
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);  // ����Ʈ����

	if (bHit)
	{
		fireEffect();  // ���� & ī�޶���ũ 
		currentBullet--;  //�Ѿ˰���

		// ����������
		auto hitComp = hitInfo.GetComponent();
		if (hitComp && hitComp->IsSimulatingPhysics())   
		{
			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;  
			hitComp->AddForce(force);    
		}

		auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));   //  FSM �� �ε������� ��
		if (enemy)
		{
			auto enemyFSM = Cast<UEnemyFSM>(enemy);
			enemyFSM->OnDamageProcess();
		}
	}
}

void ATPSPlayer::OnHitEvent()  // �ǰ� �̺�Ʈ
{
		UE_LOG(LogTemp, Warning, TEXT("Player Damaged!"));
		HP--;
		UE_LOG(LogTemp, Warning, TEXT("Player HP : %d"), HP);

		if (HP <= 0)  // HP�� 0�� �Ǹ�
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Dead!"));
			OnGameOver();  // ���� ���� �Լ� ȣ��
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