// Fill out your copyright notice in the Description page of Project Settings.
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






ATPSPlayer::ATPSPlayer()   // ������ �Լ� ���   --------------------------------------------------------------------------------
{
	PrimaryActorTick.bCanEverTick = true;

	//  1-���̷�Ż�޽� 
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Wise/Character/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));

	// 1-�����̸� ��ġ,ȸ���� ����
	if (TempMesh.Succeeded())    // 
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object); //GetMesh �Լ��� �̿��Ͽ� Mesh ������Ʈ�� ������ SetSkeletalMesh �Լ��� ȣ��.

		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));  //�޽�������Ʈ ��ġ, ȸ���� ����

	}
	// �׽�Ʈ��
	//  1-�������� 
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));  //���������Ʈ ���丮�Լ��� �̿��Ͽ� <  > �ν��Ͻ��� �����Ͽ� ������ ����
																						 // ���� =     TEXT("�����̸�")	
	springArmComp->SetupAttachment(RootComponent);      // ��Ʈ�� �ڽ�����
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));    // ������ ������  ��ġ (����)
	springArmComp->TargetArmLength = 400;         // �� ����= 400  (����)


	//  1-�̵�
	springArmComp->bUsePawnControlRotation = true;      //  ���������� ȸ���� ���ӵ� 



	// 1-ī�޶� 
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));  // ���
	tpsCamComp->SetupAttachment(springArmComp);      // ���������� �ڽ�
	tpsCamComp->bUsePawnControlRotation = false;      // ī�޶�� ȸ���� ����
	bUseControllerRotationYaw = true;      //  �÷��̾�ȸ��- Ŭ������ ����Ʈ �������� �ǹ� 

	// 1-���� ����
	JumpMaxCount = 2;

	// 2-�� �޽� 
	gunMeshComp = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("GunMeshComp"));    // ���
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));    //���̷�Ż �޽��� �ڽ�    10- ���Ϻ��̱�

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/Wise/Resources/FPWeapon/Mesh/SK_FPGun.SK_FPGun'")); //  ������ �ε� //  TempGunMesh ��������
	if (TempGunMesh.Succeeded())  // ���� �����ϸ�
	{
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);  // ���̷�Ż�޽� ������ �Ҵ�
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 13));    // �� ��ġ   10- ���Ϻ��̱�� ��ġ����
		gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));    // ȸ��  10- ���Ϻ��̱�
	}
		
	

	//5- �������� �� �޽�

	
	sniperGunComp = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("SniperGunComp"));    // ���
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));    // 5-�޽��� �ڽ�, 10- ���Ϻ��̱�

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/Wise/Resources/SniperGun/sniper1.sniper1'")); //  ������ �ε� //  TempSniperMesh ����ü(ConstructorHelpers)��������
	
	if (TempSniperMesh.Succeeded())  // ���� �����ϸ�
	{
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);  // ����ƽ�޽� ������ �Ҵ�
		sniperGunComp->SetRelativeLocation(FVector(-12, 75, 13));   // 10 �������۰� ���Ͽ� ���̱� ��ġ�ٲٱ�
		sniperGunComp->SetRelativeRotation(FRotator(0, 90, 0));     // 10 ȸ��
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));   // �� ������ 
	}

}






void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	
	ChangeToSniperGun();  // 5-  �������۰��� �⺻������
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);      // 6-2 �������� ui ���� �ν��Ͻ� ����
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);   // 6-5 �Ϲ� ���� ui ũ�ν���� �ν��Ͻ��� �����Ѵ� 
	_crosshairUI->AddToViewport();       //  �Ϲ����� UI ���

	HP = initialHP;
		
}

void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();    // ���� ȣ�� -

	currentTime += DeltaTime;  // ���� �ð��� DeltaTime ����
	
}




// �Լ� ���� -------------------------------------------------------------------------------------------------------------------

void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); // ����� �� �� �ѹ� ȣ��ȴ�- �Լ��� �Է�Ű�� ����(���ε�)�� �ش�

	// 2-1. ���ε� 
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSPlayer::Turn);  //��ǲ������Ʈ��   ���� �ִ�  �Լ��ּ�(Turn) �� �Ѱ��־� ó���ϵ��� ������ �ش�
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::InputVertical);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::InputJump);   // ���� 
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);   // 3- �߻� 
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &ATPSPlayer::ChangeToGrenadeGun);   // 5- �׷�������
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::ChangeToSniperGun);   // 5- �������۰�
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);    //6- �������� pressed
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);    //6- �������� Released

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &ATPSPlayer::InputAttack);  // ����

	PlayerInputComponent->BindAction(TEXT("Assasinate"), IE_Released, this, &ATPSPlayer::InputAssasinate);  // �ϻ�
}

	// 2-1 �¿�ȸ�� turn �Է� �Լ� ���� - yaw ȸ�� ok
void ATPSPlayer::Turn(float value)
{
	AddControllerYawInput(value);  
}

	// 2-1 ����ȸ�� LookUp  �Է� �Լ� ���� - pitch ok
void ATPSPlayer::LookUp(float value)
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
void ATPSPlayer::InputJump()      // ���� �Լ� ����- () !!!
{
	Jump();     // ���� �Լ� ����: ACharacter �Լ��� �̹� �����Լ��� �վ� ȣ�⸸ �ϸ� ��
}
void ATPSPlayer::Move()         // ���� �Լ��� ����
{

	// ���� �������� �ٶ󺸱�-
	direction = FTransform(GetControlRotation()).TransformVector(direction);   // ĳ���� �����Ʈ ������Ʈ�� Ȱ��, walkspeed �ʿ����
			 // �����̼ǰ��� �޾ƿ� direction�� �ٲٰٴ� = �̵������� ��Ʈ�ѹ��� �������� ��ȯ�Ѵ�, 
				//�̵�  P = P0 + vt
				//FVector P0 = GetActorLocation();
				//FVector vt = direction * walkSpeed * DeltaTime;
				//FVector P = P0 + vt;
				//SetActorLocation(P);

	AddMovementInput(direction);
	direction = FVector::ZeroVector;   // ���� ���� ����   ????
}



void ATPSPlayer::InputFire()   // 3- �߻� 
{
	if (bUsingGrenadeGun)  // 6- ��ź���̸� 
	{                // �Ѿ˹߻罺���ϰ�
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));    //3-�ѱ���ġ :
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);  // 3-���� : 
	}
	else     // �������۰� ����ϸ�
	{
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
		}

	}
}

void ATPSPlayer::ChangeToGrenadeGun()    // 5- �׷�����������
{
	bUsingGrenadeGun = true;   // �����  true
	sniperGunComp->SetVisibility(false);   //�������� no
	gunMeshComp->SetVisibility(true);    // �׷����� yes


}

void ATPSPlayer::ChangeToSniperGun()   // �������۰�����
{
	bUsingGrenadeGun = false;   // �����  false
	sniperGunComp->SetVisibility(true);   //�������� yes
	gunMeshComp->SetVisibility(false);    // �׷����� no
}

void ATPSPlayer::SniperAim()  // 6- �������� �Է� ������ �� ������ ��
{
	if (bUsingGrenadeGun)    // ��ź�� ������� ���� �����Ѵ�
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

void ATPSPlayer::InputAttack()  // �����ϴ� �Լ�(���콺 ��Ŭ��)
{
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AIH_Enemy::StaticClass());  // AIH_Enemy�� ã�Ƽ� actor ������ �־�д�.
	if(actor == nullptr) return;

	AIH_Enemy* enemy = Cast<AIH_Enemy>(actor);  // actor�� AIH_Enemy�� ������ enemy ������ �����Ѵ�.
	UEnemyFSM* enemyFSM = Cast<UEnemyFSM>(enemy);

	
	if (currentTime > attackDelayTime)
	{
		enemy->fsm->OnDamageProcess();
		currentTime = 0;
	}
}

void ATPSPlayer::InputAssasinate()  // �ϻ��ϴ� �Լ�(Ű���� E)
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