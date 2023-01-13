
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

public:
	// �� 
	UPROPERTY(VisibleAnywhere, Category = Camera)  
	class USpringArmComponent* springArmComp;     
	                                             
	// ī�޶� 
	UPROPERTY(VisibleAnywhere,  BlueprintReadOnly, Category = Camera)   
	class UCameraComponent* tpsCamComp;     

	// �Ѿ˰���
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)   
	TSubclassOf<class ABullet>bulletFactory;   

	// �� �޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)  
	class USkeletalMeshComponent* pistolComp;

	// Ȱ �޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)
	class USkeletalMeshComponent* bowComp;

	// Į �޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)
	class USkeletalMeshComponent* kalComp;

		


	bool bNoEquipped = true;   // �ָԸ��
	bool bSniperAim = false;     // �����������ظ��
	FHitResult hitInfo;    // �浹����

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   // ����������������
	TSubclassOf<class UUserWidget> sniperUIFactory;     

	UPROPERTY(EditAnywhere, Category = BulletEffect)   //�Ѿ� ����ȿ�� ����
		class UParticleSystem* bulletEffectFactory;  

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   // ũ�ν���� ��������
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	UPROPERTY()
		class UUserWidget* _sniperUI;          // ������������

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UUserWidget* _crosshairUI;     // ũ�ν��������
		
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // �ȱ� �ӵ�
		float walkSpeed = 375;                 
	
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // �޸��� �ӵ�
		float runSpeed = 600;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
		float crouchSpeed = 200;                            //��ũ���� �ӵ�

	UPROPERTY()
		class UPlayerAnim* anim;                         // �ִԺ���

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)     // ī�޶���ũ 
		TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditAnywhere)
		int32 currentBullet = 0;
		
// ��������
	UPROPERTY(EditAnywhere)  // �� ��������
		bool bgetGun = false;

	UPROPERTY(EditAnywhere)  // Ȱ ��������
		bool bgetbow = false;

	UPROPERTY(EditAnywhere)  // Į ��������
		bool bgetKal = false;



	UPROPERTY(EditDefaultsOnly, Category=Sound)   //�Ѿ� �߻� ����
		class USoundBase* bulletSound;

// 	UPROPERTY(EditAnywhere)
// 	bool isBack = false;  // �ڿ��� ������ �� ����ϴ� bool ����

	UPROPERTY(EditAnywhere)
		class AIH_Enemy* backEnemy = nullptr; 

	UPROPERTY(EditAnywhere)
		bool isOnAttack = false;   

	float currentTime = 0;

	UPROPERTY(EditAnywhere)
		float attackDelayTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = Health)
		int32 HP;  // ���� HP

	UPROPERTY(EditDefaultsOnly, Category = Health)
		int32 initialHP = 5;  // ���� HP

	UPROPERTY(EditAnywhere)
		class UBoxComponent* compBox;

	FVector direction = FVector::ZeroVector;  

	void Turn(float value);   
	void LookUp(float value);    
	void InputHorizontal(float value); 
	void InputVertical(float value);    
	void InputJump();   
	void Move();      // ����
	void InputFire();   //  �߻�
	void InputRun();   //  �޸���
	void InputCrouch(); // ���̱�
//	void InputAttack(); // ����
	void InputAssasinate();  // �ϻ�
	void fireEffect(); // �� ����Ʈ  
	void LineTrace();  // ����Ʈ���̽�
	void ChangeToNoEquipped();    // �ָ�
	void ChangeToPistol();     // ����
	void ChangeToBow();    // Ȱ
	void ChangeToKal();    // Į
	void SniperAim();     //����


	UFUNCTION(BlueprintCallable, Category = Health)
		void OnHitEvent();  // �ǰ� �̺�Ʈ
	UFUNCTION(BlueprintCallable, Category = Health)
		void OnGameOver();  // GameOver �̺�Ʈ
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
