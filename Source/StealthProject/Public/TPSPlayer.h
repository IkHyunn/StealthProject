
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class STEALTHPROJECT_API ATPSPlayer : public ACharacter  
{
	GENERATED_BODY()
public:
	ATPSPlayer();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;  

public:  // �޽�, �ø���
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)  // �� 
	class USpringArmComponent* springArmComp;     
	                                             
	UPROPERTY(VisibleAnywhere,  BlueprintReadWrite, Category = Camera)   // ī�޶� 
	class UCameraComponent* tpsCamComp;     
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh)  // Ȱ �޽�
	class USkeletalMeshComponent* bowComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh)   // Į �޽�
	class USkeletalMeshComponent* kalComp;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* righthandBox;      // ������ Collision

	UPROPERTY(EditAnywhere)
	class UBoxComponent* lefthandBox;  // �޼� Collision

	UPROPERTY(EditAnywhere)
	class UBoxComponent* knifeBox;  // Į Collision


public: // �����, ������, subclass
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)   // �Ѿ˰���
		TSubclassOf<class ABullet>bulletFactory;   

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   // ����������������
		TSubclassOf<class UUserWidget> sniperUIFactory;     

	UPROPERTY(EditAnywhere, Category = BulletEffect)   //�Ѿ� ����ȿ�� ����
		class UParticleSystem* bulletEffectFactory;  

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)     // ī�޶���ũ 
		TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditDefaultsOnly, Category=Sound)   //�Ѿ� �߻� ����
		class USoundBase* bulletSound;

	UPROPERTY()
		class UUserWidget* _sniperUI;          // ������������

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCrosshairUI> crosshairFactory;

	UPROPERTY(EditAnywhere)
		class UCrosshairUI* crosshairUI;     // ũ�ν���� ����

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UArrowCountUI> arrowcountFactory;

	UPROPERTY(EditAnywhere)
		class UArrowCountUI* arrowcountUI;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UPlayerHP> playerHPFactory;

	UPROPERTY(EditAnywhere)
		class UPlayerHP* playerHPUI;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UPlusCountUI> pluscountFactory;

	UPROPERTY(EditAnywhere)
		class UPlusCountUI* plusCountUI;


public: // �ð�, �ӵ�, hp  ���� ��..
		
	float currentTime = 0;                     // �ð�
	FVector direction = FVector::ZeroVector;  

	UPROPERTY(EditAnywhere)                 // ���� �Ѿ˰���
		int32 currentArrow = 0;

	UPROPERTY(EditAnywhere)                 // ���� �����̽ð�
		float attackDelayTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = Health)    // ���� HP
		float HP;  

	UPROPERTY(EditDefaultsOnly, Category = Health)    // ���� HP
		float maxHP = 5;
		
	UPROPERTY(EditDefaultsOnly, Category = Health)
		float prevHP;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // �ȱ� �ӵ�
		float walkSpeed = 375;                 
	
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // �޸��� �ӵ�
		float runSpeed = 600;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)     //��ũ���� �ӵ�
		float crouchSpeed = 300;                           

	UPROPERTY(EditAnywhere, Category = PlayerSetting)  // �� �� ���� �Ÿ�
		float zoomIn = 90;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)  // �� �ƿ� ���� �Ÿ�
		float zoomOut = 75;

	UPROPERTY(EditAnywhere)                 // �޺� ����
		int32 attackCount = 0;

// ĳ��Ʈ ������

	UPROPERTY()
		class UPlayerAnim* anim;                         // �÷��̾� �ִϸ��̼� ĳ��Ʈ

	UPROPERTY(EditAnywhere)
		class AIH_Enemy* backEnemy = nullptr;            // ���ʹ�
		
	FHitResult hitInfo;    // �浹����

	UPROPERTY(EditAnywhere)
		class AIH_HPItem* pickHPItem = nullptr;

	UPROPERTY(EditAnywhere)
		class AKal* pickKnife = nullptr;

	UPROPERTY(EditAnywhere)
		class AIH_Bullet* pickArrow = nullptr;

	UPROPERTY(EditAnywhere)
		class AIH_Knife* pickBow = nullptr;


// ���� ������ ���̱� ������������
	UPROPERTY(EditAnywhere) 
		bool bgetGun = false;    // �� ��������

	UPROPERTY(EditAnywhere)  
		bool bgetbow = false;   // Ȱ ��������

	UPROPERTY(EditAnywhere)  
		bool bgetKal = false;  // Į ��������

		bool bPunch = true;   // ���ָ� ���� (������ ������ �÷��̾�� ���ָ� �����̱� ������ �⺻���� true)

		bool bSniperAim = false;    // �����������ظ��   ???

 	UPROPERTY(EditAnywhere)
 		bool isBack = false;  // �ڿ��� ������ �� ����ϴ� bool ����

	UPROPERTY(EditAnywhere)
		bool bCrouched = false;  // ��ũ���� �ִ���

	UPROPERTY(EditAnywhere)
		bool isAttacking = false; 

	UPROPERTY(EditAnywhere)
		bool saveAttack = false;

	UPROPERTY(EditAnywhere)
		class APlayerController* playerController;

// ���� ����
	UPROPERTY(EditAnywhere)
		class USoundBase* bowLoadingSound;

	UPROPERTY(EditAnywhere)
		class USoundBase* pickKnifeSound;

	UPROPERTY(EditAnywhere)
		class USoundBase* pickArrowSound;

	UPROPERTY(EditAnywhere)
		class USoundBase* pickBowSound;

	UPROPERTY(EditAnywhere)
		class USoundBase* swordDamagedSound;
	
	UPROPERTY(EditAnywhere)
		class USoundBase* punchDamagedSound;
	

public: //  �Լ�
	void Turn(float value);   
	void LookUp(float value);    
	void InputHorizontal(float value); 
	void InputVertical(float value);    
	void Move();      // �̵�
	void InputJump();   
	void InputFire();   //  �߻� 
	void InputRun();   //  �޸���
	void InputCrouch(); // ���̱�
	void SniperAim();     //����
	void InputAssasinate();  // �ϻ�
	void ChangeToPunch();    // �ָ�
	void ChangeToBow();    // Ȱ
	void ChangeToKal();    // Į
	void InputMission();
	void InputPickUp();
	void fireEffect(); // �� ����Ʈ  
	void LineTrace();  // ����Ʈ���̽�

	void ComboAttackSave();
	void ComboReset();
	
	void ReceiveDamage(float damage);

// UFUNCTION
	UFUNCTION(BlueprintCallable, Category = Health)
		void OnHitEvent();  // �ǰ� �̺�Ʈ
	UFUNCTION(BlueprintCallable, Category = Health)
		void OnGameOver();  // GameOver �̺�Ʈ
	UFUNCTION()
		void HandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void LeftHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void KnifeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
