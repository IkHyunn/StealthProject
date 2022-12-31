
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class STEALTHPROJECT_API ATPSPlayer : public ACharacter   //
{
	GENERATED_BODY()

public:
	ATPSPlayer();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// �����Լ� ����� ----------------------------------------------------------------------------------------

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;  //���� ��ӹ޾� �ڵ����� �������̵� �Ѵ�- 
	


public:
	// ������Ʈ ������� ����� ------------------------------------------------------------------------------
	
	// 1- �������� 
	UPROPERTY(VisibleAnywhere, Category = Camera)   // Ŭ���� ���漱��, (#include ���� �ʰ�, ������ ������ �ϱ� ����) & ������ ���� �տ��� class ����,
	class USpringArmComponent* springArmComp;     // �����ͺ���, �޸𸮰����� �𸮾�κ��� �������� ���� �� �ֵ��� uproperty ������
	                                             //class Ÿ�� *�����ͺ���
	// 1- ī�޶� 
	UPROPERTY(VisibleAnywhere,  BlueprintReadOnly, Category = Camera)   // 6- �������� ������ ���� ����READONLY
	class UCameraComponent* tpsCamComp;      //

// 	// 2- �� �޽�
// 	UPROPERTY(VisibleAnywhere, Category = GunMesh)  //
// 	class USkeletalMeshComponent* gunMeshComp;

	// 3- �Ѿ˰���
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)    // 
	TSubclassOf<class ABullet>bulletFactory;    // ���ø��� ��ϵ� �������Ʈ Ŭ������ �����ͷκ��� �Ҵ�޴´�  ??

	// 4- �������� �� �޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)  //
	class USkeletalMeshComponent* pistolComp;

	// 5- ���� ��ź���� ����ϰ� �ִ��� 
	bool bNoEquipped = true;   // bool ����
	void ChangeToNoEquipped();    // �׷�����������
	void ChangeToPistol();     // �������۰�����

	// 6- �������� ����
	void SniperAim();     //
	bool bSniperAim = false;     // 6-1 �������� ����( press ���� release ����) ����

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)    //  6-2 �������� UI ������ ���� ����
	TSubclassOf<class UUserWidget> sniperUIFactory;     

	UPROPERTY()
	class UUserWidget* _sniperUI;    // �������� ui ���� �ν��Ͻ�

	UPROPERTY(EditAnywhere, Category = BulletEffect)   // 6-3 �Ѿ� ����ȿ�� ����
	class UParticleSystem* bulletEffectFactory;  

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   //  6-4 �Ϲ� ���� ũ�ν����UI �Ӽ� �߰�
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUserWidget* _crosshairUI;     // ũ�ν���� �ν��Ͻ�
		
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // 10-�ȱ� �ӵ�
	float walkSpeed = 375;                 
	
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // 10- �޸��� �ӵ�
	float runSpeed = 600;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float crouchSpeed = 200;

	UPROPERTY()
	class UPlayerAnim* anim;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)     //ī�޶���ũ �������Ʈ�� ������ ����
	TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditAnywhere)
	int32 currentBullet = 0;

	UPROPERTY(EditAnywhere)
	bool bgetGun = false;

	UPROPERTY(EditDefaultsOnly, Category=Sound)   //�Ѿ� �߻� ����
	class USoundBase* bulletSound;

	UPROPERTY(EditAnywhere)
	bool isBack = false;  // �ڿ��� ������ �� ����ϴ� bool ����

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

	
	// �Լ� ����� ---------------------------------------------------------------------------

	void Turn(float value);    // 1-�¿�ȸ��
	void LookUp(float value);    //  1- ����ȸ��
	FVector direction = FVector::ZeroVector;   //  1- ���� direction = ���� �ʱ�ȭ (FVector::ZeroVector) , ��������� ����,���ͺ���  ??
	void InputHorizontal(float value);  //  1- �¿��̵�
	void InputVertical(float value);     //  1- �����̵�
	void InputJump();   // 1- ����
	void Move();      //. 1- ����
	void InputFire();   //  3- �߻�
	void InputRun();   // 10- �޸���
	void InputCrouch(); // ���̱�

//	void InputAttack(); // ����
	void InputAssasinate();  // �ϻ�

	UFUNCTION(BlueprintCallable, Category = Health)
		void OnHitEvent();  // �ǰ� �̺�Ʈ

	UFUNCTION(BlueprintCallable, Category = Health)
		void OnGameOver();  // GameOver �̺�Ʈ
		 
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
