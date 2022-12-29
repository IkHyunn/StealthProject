
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

	// 2- �� �޽�
	UPROPERTY(VisibleAnywhere, Category = GunMesh)  //
	class USkeletalMeshComponent* gunMeshComp;

	// 3- �Ѿ˰���
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)    // 
	TSubclassOf<class ABullet>bulletFactory;    // ���ø��� ��ϵ� �������Ʈ Ŭ������ �����ͷκ��� �Ҵ�޴´�  ??

	// 4- �������� �� �޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)  //
	class UStaticMeshComponent* sniperGunComp;

	// 5- ���� ��ź���� ����ϰ� �ִ��� 
	bool bUsingGrenadeGun = true;   // bool ����
	void ChangeToGrenadeGun();    // �׷�����������
	void ChangeToSniperGun();     // �������۰�����

	// 6- �������� ����
	void SniperAim();     //
	bool bSniperAim = false;     // 6-1 �������� ����( press ���� release ����) ����

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)    //  6-2 �������� UI ��������
	TSubclassOf<class UUserWidget> sniperUIFactory;      // UI���丮    ????
	UPROPERTY()
	class UUserWidget* _sniperUI;     // �������� ui ���� �ν��Ͻ�

	UPROPERTY(EditAnywhere, Category = BulletEffect)    // 6-3 �Ѿ� ����ȿ�� ����
	class UParticleSystem* bulletEffectFactory;  

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)    //  6-4 �Ϲ� ���� ũ�ν����UI �Ӽ� �߰�
	TSubclassOf<class UUserWidget> crosshairUIFactory;
	class UUserWidget* _crosshairUI;                     // ũ�ν���� �ν��Ͻ�

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

	UFUNCTION(BlueprintCallable, Category = Health)
		void OnHitEvent();  // �ǰ� �̺�Ʈ

	UFUNCTION(BlueprintCallable, Category = Health)
		void OnGameOver();  // GameOver �̺�Ʈ

	
	// �Լ� ����� ---------------------------------------------------------------------------

	void Turn(float value);    // 1-�¿�ȸ��
	void LookUp(float value);    //  1- ����ȸ��
	FVector direction = FVector::ZeroVector;   //  1- ���� direction = ���� �ʱ�ȭ (FVector::ZeroVector) , ��������� ����,���ͺ���  ??
	void InputHorizontal(float value);  //  1- �¿��̵�
	void InputVertical(float value);     //  1- �����̵�
	void InputJump();   // 1- ����
	void Move();      //. 1- ����
	void InputFire();   //  3- �߻�

	void InputAttack(); // ����
	void InputAssasinate();  // �ϻ�

		 

};
