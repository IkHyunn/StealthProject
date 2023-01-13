
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
	// 암 
	UPROPERTY(VisibleAnywhere, Category = Camera)  
	class USpringArmComponent* springArmComp;     
	                                             
	// 카메라 
	UPROPERTY(VisibleAnywhere,  BlueprintReadOnly, Category = Camera)   
	class UCameraComponent* tpsCamComp;     

	// 총알공장
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)   
	TSubclassOf<class ABullet>bulletFactory;   

	// 총 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)  
	class USkeletalMeshComponent* pistolComp;

	// 활 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)
	class USkeletalMeshComponent* bowComp;

	// 칼 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)
	class USkeletalMeshComponent* kalComp;

		


	bool bNoEquipped = true;   // 주먹모드
	bool bSniperAim = false;     // 스나이퍼조준모드
	FHitResult hitInfo;    // 충돌정보

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   // 스나이퍼위젯공장
	TSubclassOf<class UUserWidget> sniperUIFactory;     

	UPROPERTY(EditAnywhere, Category = BulletEffect)   //총알 파편효과 공장
		class UParticleSystem* bulletEffectFactory;  

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   // 크로스헤어 위젯공장
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	UPROPERTY()
		class UUserWidget* _sniperUI;          // 스나이퍼위젯

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UUserWidget* _crosshairUI;     // 크로스헤어위젯
		
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // 걷기 속도
		float walkSpeed = 375;                 
	
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // 달리기 속도
		float runSpeed = 600;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
		float crouchSpeed = 200;                            //웅크리기 속도

	UPROPERTY()
		class UPlayerAnim* anim;                         // 애님변수

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)     // 카메라세이크 
		TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditAnywhere)
		int32 currentBullet = 0;
		
// 가짐여부
	UPROPERTY(EditAnywhere)  // 총 가짐여부
		bool bgetGun = false;

	UPROPERTY(EditAnywhere)  // 활 가짐여부
		bool bgetbow = false;

	UPROPERTY(EditAnywhere)  // 칼 가짐여부
		bool bgetKal = false;



	UPROPERTY(EditDefaultsOnly, Category=Sound)   //총알 발사 사운드
		class USoundBase* bulletSound;

// 	UPROPERTY(EditAnywhere)
// 	bool isBack = false;  // 뒤에서 공격할 때 사용하는 bool 변수

	UPROPERTY(EditAnywhere)
		class AIH_Enemy* backEnemy = nullptr; 

	UPROPERTY(EditAnywhere)
		bool isOnAttack = false;   

	float currentTime = 0;

	UPROPERTY(EditAnywhere)
		float attackDelayTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = Health)
		int32 HP;  // 현재 HP

	UPROPERTY(EditDefaultsOnly, Category = Health)
		int32 initialHP = 5;  // 최초 HP

	UPROPERTY(EditAnywhere)
		class UBoxComponent* compBox;

	FVector direction = FVector::ZeroVector;  

	void Turn(float value);   
	void LookUp(float value);    
	void InputHorizontal(float value); 
	void InputVertical(float value);    
	void InputJump();   
	void Move();      // 무브
	void InputFire();   //  발사
	void InputRun();   //  달리기
	void InputCrouch(); // 숙이기
//	void InputAttack(); // 공격
	void InputAssasinate();  // 암살
	void fireEffect(); // 총 이펙트  
	void LineTrace();  // 라인트레이스
	void ChangeToNoEquipped();    // 주먹
	void ChangeToPistol();     // 권총
	void ChangeToBow();    // 활
	void ChangeToKal();    // 칼
	void SniperAim();     //조준


	UFUNCTION(BlueprintCallable, Category = Health)
		void OnHitEvent();  // 피격 이벤트
	UFUNCTION(BlueprintCallable, Category = Health)
		void OnGameOver();  // GameOver 이벤트
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
