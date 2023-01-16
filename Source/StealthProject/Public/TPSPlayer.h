
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh)  // �� �޽�
	class USkeletalMeshComponent* pistolComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh)  // Ȱ �޽�
	class USkeletalMeshComponent* bowComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh)   // Į �޽�
	class USkeletalMeshComponent* kalComp;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* compBox;      // ���ø��� ??


public: // �����, ������, subclass
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)   // �Ѿ˰���
		TSubclassOf<class ABullet>bulletFactory;   

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   // ����������������
		TSubclassOf<class UUserWidget> sniperUIFactory;     

	UPROPERTY(EditAnywhere, Category = BulletEffect)   //�Ѿ� ����ȿ�� ����
		class UParticleSystem* bulletEffectFactory;  

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   // ũ�ν���� ��������
		TSubclassOf<class UUserWidget> crosshairUIFactory;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)     // ī�޶���ũ 
		TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditDefaultsOnly, Category=Sound)   //�Ѿ� �߻� ����
		class USoundBase* bulletSound;

	UPROPERTY()
		class UUserWidget* _sniperUI;          // ������������

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UUserWidget* _crosshairUI;     // ũ�ν��������


public: // �ð�, �ӵ�, hp  ���� ��..
		
	float currentTime = 0;                     // �ð�
	FVector direction = FVector::ZeroVector;  

	UPROPERTY(EditAnywhere)                 // ���� �Ѿ˰���
		int32 currentBullet = 0;

	UPROPERTY(EditAnywhere)                 // ���� �����̽ð�
		float attackDelayTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = Health)    // ���� HP
		int32 HP;  

	UPROPERTY(EditDefaultsOnly, Category = Health)    // ���� HP
		int32 initialHP = 5;  

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

// ĳ��Ʈ ������

	UPROPERTY()
		class UPlayerAnim* anim;                         // �÷��̾� �ִϸ��̼� ĳ��Ʈ

	UPROPERTY(EditAnywhere)
		class AIH_Enemy* backEnemy = nullptr;            // ���ʹ�
		
	FHitResult hitInfo;    // �浹����


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
		bool isOnAttack = false;   

	UPROPERTY(EditAnywhere)
		class APlayerController* playerController;

	

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
//	void InputAttack(); // ����
	void SniperAim();     //����
	void InputAssasinate();  // �ϻ�
	void ChangeToPunch();    // �ָ�
	void ChangeToPistol();     // ����
	void ChangeToBow();    // Ȱ
	void ChangeToKal();    // Į
	void fireEffect(); // �� ����Ʈ  
	void LineTrace();  // ����Ʈ���̽�

// UFUNCTION
	UFUNCTION(BlueprintCallable, Category = Health)
		void OnHitEvent();  // �ǰ� �̺�Ʈ
	UFUNCTION(BlueprintCallable, Category = Health)
		void OnGameOver();  // GameOver �̺�Ʈ
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
