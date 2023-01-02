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






ATPSPlayer::ATPSPlayer()   // ������ �Լ� ���   --------------------------------------------------------------------------------
{
	PrimaryActorTick.bCanEverTick = true;

	//  1-���̷�Ż�޽� 
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Wise/Character/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())  
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object); //GetMesh �Լ��� �̿��Ͽ� Mesh ������Ʈ�� ������ SetSkeletalMesh �Լ��� ȣ��.

		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));  //��ġ, ȸ���� 
	}

	//  1-�������� 
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));  //���������Ʈ ���丮�Լ��� �̿��Ͽ� <  > �ν��Ͻ��� �����Ͽ� ������ ����
	springArmComp->SetupAttachment(RootComponent);      // ��Ʈ�� �ڽ�����
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));    // ��ġ
	springArmComp->TargetArmLength = 400;         // �� ����


	//  1-�̵�
	springArmComp->bUsePawnControlRotation = true;      //  ���������� ȸ���� ���ӵ� 



	// 1-ī�޶� 
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));  // ���
	tpsCamComp->SetupAttachment(springArmComp);      // ���������� �ڽ�
	tpsCamComp->bUsePawnControlRotation = false;      // ī�޶�� ȸ���� ����
	bUseControllerRotationYaw = true;      //  �÷��̾�ȸ��- Ŭ������ ����Ʈ �������� �ǹ� 

	// 1-���� ����
	JumpMaxCount = 2;

// 	// 2-�� �޽� 
// 	gunMeshComp = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("GunMeshComp"));    // ���
// 	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));    //���̷�Ż �޽��� �ڽ�    10- ���Ϻ��̱�
// 
// 	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/FPWeapon/Mesh/SK_FPGun.SK_FPGun'")); //  ������ �ε� //  TempGunMesh ��������
// 	if (TempGunMesh.Succeeded())  // ���� �����ϸ�
// 	{
// 		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);  // ���̷�Ż�޽� ������ �Ҵ�
// 		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 13));    // �� ��ġ   10- ���Ϻ��̱�� ��ġ����
// 		gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));    // ȸ��  10- ���Ϻ��̱�
// 	}
		
	

	//5- �������� �� �޽�

	pistolComp = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("SniperGunComp"));    // ���
	pistolComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));    // 5-�޽��� �ڽ�, 10- ���Ϻ��̱�

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempPistolMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/MilitaryWeapSilver/Weapons/Pistols_A.Pistols_A'")); //  ������ �ε� //  TempSniperMesh ����ü(ConstructorHelpers)��������
	
	if (TempPistolMesh.Succeeded())  // ���� �����ϸ�
	{
		pistolComp->SetSkeletalMesh(TempPistolMesh.Object);  // ����ƽ�޽� ������ �Ҵ�
		pistolComp->SetRelativeLocation(FVector(-12, 75, 13));   // 10 �������۰� ���Ͽ� ���̱� ��ġ�ٲٱ�
		pistolComp->SetRelativeRotation(FRotator(0, 90, 0));     // 10 ȸ��
	}


	// ��������ui ���� �������Ʈ Ŭ���� ��������
	ConstructorHelpers::FClassFinder<UUserWidget> _tempsniperUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_SniperUI.WBP_SniperUI_C'"));
	if (_tempsniperUI.Succeeded())
	{
		sniperUIFactory = _tempsniperUI.Class;
	}


	// ũ�ν���� ���� �������Ʈ Ŭ���� ��������
	ConstructorHelpers::FClassFinder<UUserWidget> tempUI(TEXT("WidgetBlueprint'/Game/Wise/Widget/WBP_Crosshair.WBP_Crosshair_C'"));
	if (tempUI.Succeeded())
	{
		crosshairUIFactory = tempUI.Class;
	}


	//  �Ѿ� �������Ʈ Ŭ���� ��������
	ConstructorHelpers::FClassFinder<ABullet> tempbulletFactory(TEXT("Blueprint'/Game/Wise/Blueprints/BP_Bullet.BP_Bullet_C'"));
	if (tempbulletFactory.Succeeded())
	{
		bulletFactory = tempbulletFactory.Class;
	}

	// ������ ��ƼŬ ������Ʈ ��������
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempEffectFactory(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_BulletEffect.P_BulletEffect'"));
	if (tempEffectFactory.Succeeded())
	{
		bulletEffectFactory = tempEffectFactory.Object;
	}

	//UPROPERTY(EditDefaultsOnly, Category = CameraMotion)     //ī�޶���ũ �������Ʈ�� ������ ����
	//	TSubclassOf<class UCameraShakeBase> cameraShake;

	// ī�޶� ����ũ �������Ʈ ��������
	ConstructorHelpers::FClassFinder<UCameraShakeBase> tempShakeBase(TEXT("Blueprint'/Game/Wise/Blueprints/BP_CamerShake.BP_CamerShake_C'"));
	if (tempShakeBase.Succeeded())
	{
		cameraShake = tempShakeBase.Class;
	}


	// �� ���� ������Ʈ ��������
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
//	ChangeToNoEquipped();  // �������۰��� �⺻������
	anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());   // �ִϸ��̼� �������Ʈ ����
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;           // ������ �ȱ��
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      // ��������ui��������
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);    // ũ�ν������������

	compBox -> OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::OnOverlap);

	HP = initialHP;
}

void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();   // �̵�

	currentTime += DeltaTime;   
	
}




// �Լ� ���� -------------------------------------------------------------------------------------------------------------------

void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); // ����� �� �� �ѹ� ȣ��ȴ�- �Լ��� �Է�Ű�� ����(���ε�)�� �ش�

	// 2-1. ���ε� 
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);  
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::InputVertical);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::InputJump);   // ���� 
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);   // �߻� 
	PlayerInputComponent->BindAction(TEXT("NoEquipped"), IE_Pressed, this, &ATPSPlayer::ChangeToNoEquipped);   // �׷�����������
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToPistol);   // �������۰�����
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);    // �������� 
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);    // �������� 
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATPSPlayer::InputRun); // �޸���
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATPSPlayer::InputRun);   // �ȱ�
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ATPSPlayer::InputCrouch); // ���̱�
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ATPSPlayer::InputCrouch);


//	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &ATPSPlayer::InputAttack);  // ����
	PlayerInputComponent->BindAction(TEXT("Assasinate"), IE_Released, this, &ATPSPlayer::InputAssasinate);  // �ϻ�
}

	
void ATPSPlayer::Turn(float value)   // �¿�ȸ��
{
	AddControllerYawInput(value);  
}
	
void ATPSPlayer::LookUp(float value)    // ���Ʒ� ȸ��
{
	AddControllerPitchInput(value); 
}

void ATPSPlayer::InputHorizontal(float value)  // �¿��̵� �Լ�����
{
	direction.Y = value;
}

void ATPSPlayer::InputVertical(float value)     // �����̵� �Լ�����
{
	direction.X = value;
}

void ATPSPlayer::InputJump()     // ����
{
	Jump();     
}

void ATPSPlayer::Move()         // ���� 
{
	// ���� �������� �ٶ󺸱�-
	direction = FTransform(GetControlRotation()).TransformVector(direction);   // ĳ���� �����Ʈ ������Ʈ�� Ȱ��, walkspeed �ʿ����
			 // �����̼ǰ��� �޾ƿ� �𷺼��� �ٲٰٴ� = �̵������� ��Ʈ�ѹ��� �������� ��ȯ�Ѵ�, 
				//�̵�  P = P0 + vt
				//FVector P0 = GetActorLocation();
				//FVector vt = direction * walkSpeed * DeltaTime;
				//FVector P = P0 + vt;
				//SetActorLocation(P);

	AddMovementInput(direction);
	direction = FVector::ZeroVector;   // ���� ���� ????
}

void ATPSPlayer::InputRun()        // �޸���
{
	auto movement = GetCharacterMovement();
	if (bNoEquipped == true)
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

void ATPSPlayer::InputFire()   // �߻� 
{
	if (bNoEquipped)  // ���� ���¸�
	{              
		if (currentTime > attackDelayTime)
		{
			anim->isPlayerAttack = true;
			anim->PlayPunchAnim();
			currentTime = 0;
		}
	}
	else     // �������۰�
	{
		if (pistolComp->IsVisible() == true)
		{
			if (currentBullet > 0)
			{
				anim->PlayAttackAnim();  // ���� �ִϸ��̼� ���

				auto controller = GetWorld()->GetFirstPlayerController();   // ī�޶� ����ũ ���
				controller->PlayerCameraManager->StartCameraShake(cameraShake);

				FVector startPos = tpsCamComp->GetComponentLocation();  // ����Ʈ���̽��� ���� ��ġ
				FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;  // ����Ʈ���̽��� ������ġ
				FHitResult hitInfo;       // ����Ʈ���̽��� �浹������ ���� ����
				FCollisionQueryParams params;     //  �浹 �ɼ� ���� ����
				params.AddIgnoredActor(this);      // �ڱ� �ڽ�(�÷��̾�)�� �浹���� ����
	
				// ä�� ���͸� �̿��� ����Ʈ���̽� �浹 ����(�浹����,������ġ, ������ġ,����ä��,�浹�ɼ�)�ؼ� �浹�� �����ϸ� ������ true���� ���´�
				bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

				// ���࿡ ����Ʈ���̽��� �ε����� �� �浹ó�� �� �Ѿ� ����ȿ���� ����Ѵ�
				if (bHit)
				{
					FTransform bulletTrans;   // �Ѿ����� ȿ�� Ʈ������
					bulletTrans.SetLocation(hitInfo.ImpactPoint);     // �ε��� ��ġ �Ҵ�
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);  // �Ѿ�����ȿ���ν��Ͻ� ����

					auto hitComp = hitInfo.GetComponent();     //6-4 �ε��� ��ü�� ������Ʈ ������ �����Ѵ�
					if (hitComp && hitComp->IsSimulatingPhysics())   //���� ������Ʈ�� ������ ����Ǿ� �ִٸ�
					{
						FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;   // 2.�������� ���� ������ �ʿ�
						hitComp->AddForce(force);    //3. �� �������� ����������
					}

					auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
					if (enemy)
					{
						auto enemyFSM = Cast<UEnemyFSM>(enemy);
						enemyFSM->OnDamageProcess();
					}

					UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);   // �߻� ���� ���
					currentBullet--;
					UE_LOG(LogTemp, Warning, (TEXT("Current Bullet : %d")), currentBullet);
				}
				else
				{
					UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);   // �߻� ���� ���
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

void ATPSPlayer::ChangeToNoEquipped()    // 5- ���� ���·�
{
	_crosshairUI -> RemoveFromParent(); // ũ�ν���� UI �����
	pistolComp->SetVisibility(false);   // ���� no
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;  // �ӵ��� �ȴ� �ӵ���
	anim->isGunEquipped = false;
	bNoEquipped = true;
}

void ATPSPlayer::ChangeToPistol()   // ��������
{
	if (bgetGun == true)  // ���� �ֿ��� �Լ� ȣ�� ����
	{
		bNoEquipped = false;  // ���� ���°� �ƴϰ�
		GetCharacterMovement()->MaxWalkSpeed=300;  // �ȴ� �ӵ��� 300���� ����
		_crosshairUI->AddToViewport();
		anim->isGunEquipped = true;  // ���� �� �ִϸ��̼� ����
		pistolComp->SetVisibility(true);   //���� yes
	}
	else return;
}

void ATPSPlayer::SniperAim()  // 6- �������� �Է� ������ �� ������ ��
{
	if (bNoEquipped == true)    // ���� ���� ���� �����Ѵ�
	{
		return;
	}
	// pressed ��  :
	if (bSniperAim == false)  
	{
		bSniperAim = true;    // �ʱⰪ�� false �����Ƿ� true �� �ٲٰ�
		_sniperUI->AddToViewport();     // UI ȭ�鿡 ���̰�
		tpsCamComp->SetFieldOfView(45.0f);     // ī�޶�� 45�� ����
		_crosshairUI->RemoveFromParent();  //  6- �Ϲ����� ui �Ⱥ��̰�
	}
	// released ��  
	else
	{
		bSniperAim = false;     // 
		_sniperUI->RemoveFromParent();    // ui �Ⱥ��̰�
		tpsCamComp->SetFieldOfView(90.0f);   //ī�޶� �þ߰� ������� 90
		_crosshairUI->AddToViewport();     //6- �Ϲ����� ui ���
	}
	// ������ ������Ʈ�� �ִ� source ������ ������Ʈ.buld.cs ����Ŭ�� ��� ������ �ȿ��� umg ����� �߰��� �־�� �Ѵ�
	//  addRange() "UMG" �߰� 
}

// void ATPSPlayer::InputAttack()  // �����ϴ� �Լ�(���콺 ��Ŭ��)
// {
// 	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Enemy::StaticClass());  // AIH_Enemy�� ã�Ƽ� actor ������ �־�д�.
// 	if(actor == nullptr) return;
// 
// 	AIH_Enemy* enemy = Cast<AIH_Enemy>(actor);  // actor�� AIH_Enemy�� ������ enemy ������ �����Ѵ�.
// 	UEnemyFSM* enemyFSM = Cast<UEnemyFSM>(enemy);
// 
// 	
// 	if (currentTime > attackDelayTime)
// 	{
// 		enemy->fsm->OnDamageProcess();
// 		currentTime = 0;
// 	}
// }

void ATPSPlayer::InputAssasinate()  // �ϻ��ϴ� �Լ�(Ű���� E)
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