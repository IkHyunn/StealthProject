
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

public:  // 메시, 컬리젼
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)  // 암 
	class USpringArmComponent* springArmComp;     
	                                             
	UPROPERTY(VisibleAnywhere,  BlueprintReadWrite, Category = Camera)   // 카메라 
	class UCameraComponent* tpsCamComp;     
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh)  // 총 메시
	class USkeletalMeshComponent* pistolComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh)  // 활 메시
	class USkeletalMeshComponent* bowComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh)   // 칼 메시
	class USkeletalMeshComponent* kalComp;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* compBox;      // 손컬리젼 ??


public: // 공장들, 위젯들, subclass
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)   // 총알공장
		TSubclassOf<class ABullet>bulletFactory;   

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   // 스나이퍼위젯공장
		TSubclassOf<class UUserWidget> sniperUIFactory;     

	UPROPERTY(EditAnywhere, Category = BulletEffect)   //총알 파편효과 공장
		class UParticleSystem* bulletEffectFactory;  

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   // 크로스헤어 위젯공장
		TSubclassOf<class UUserWidget> crosshairUIFactory;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)     // 카메라세이크 
		TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditDefaultsOnly, Category=Sound)   //총알 발사 사운드
		class USoundBase* bulletSound;

	UPROPERTY()
		class UUserWidget* _sniperUI;          // 스나이퍼위젯

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UUserWidget* _crosshairUI;     // 크로스헤어위젯


public: // 시간, 속도, hp  변수 등..
		
	float currentTime = 0;                     // 시간
	FVector direction = FVector::ZeroVector;  

	UPROPERTY(EditAnywhere)                 // 현재 총알개수
		int32 currentBullet = 0;

	UPROPERTY(EditAnywhere)                 // 공격 딜레이시간
		float attackDelayTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = Health)    // 현재 HP
		int32 HP;  

	UPROPERTY(EditDefaultsOnly, Category = Health)    // 최초 HP
		int32 initialHP = 5;  

	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // 걷기 속도
		float walkSpeed = 375;                 
	
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // 달리기 속도
		float runSpeed = 600;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)     //웅크리기 속도
		float crouchSpeed = 300;                           

	UPROPERTY(EditAnywhere, Category = PlayerSetting)  // 줌 인 시작 거리
		float zoomIn = 90;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)  // 줌 아웃 시작 거리
		float zoomOut = 75;

// 캐스트 변수들

	UPROPERTY()
		class UPlayerAnim* anim;                         // 플레이어 애니메이션 캐스트

	UPROPERTY(EditAnywhere)
		class AIH_Enemy* backEnemy = nullptr;            // 에너미
		
	FHitResult hitInfo;    // 충돌정보


// 무기 아이템 보이기 여부판정위한
	UPROPERTY(EditAnywhere) 
		bool bgetGun = false;    // 총 가짐여부

	UPROPERTY(EditAnywhere)  
		bool bgetbow = false;   // 활 가짐여부

	UPROPERTY(EditAnywhere)  
		bool bgetKal = false;  // 칼 가짐여부

		bool bPunch = true;   // 맨주먹 여부 (시작할 때부터 플레이어는 맨주먹 상태이기 때문에 기본값이 true)

		bool bSniperAim = false;    // 스나이퍼조준모드   ???

 	UPROPERTY(EditAnywhere)
 		bool isBack = false;  // 뒤에서 공격할 때 사용하는 bool 변수

	UPROPERTY(EditAnywhere)
		bool bCrouched = false;  // 웅크리고 있는지

	UPROPERTY(EditAnywhere)
		bool isOnAttack = false;   

	UPROPERTY(EditAnywhere)
		class APlayerController* playerController;

	

public: //  함수
	void Turn(float value);   
	void LookUp(float value);    
	void InputHorizontal(float value); 
	void InputVertical(float value);    
	void Move();      // 이동
	void InputJump();   
	void InputFire();   //  발사 
	void InputRun();   //  달리기
	void InputCrouch(); // 숙이기
//	void InputAttack(); // 공격
	void SniperAim();     //조준
	void InputAssasinate();  // 암살
	void ChangeToPunch();    // 주먹
	void ChangeToPistol();     // 권총
	void ChangeToBow();    // 활
	void ChangeToKal();    // 칼
	void fireEffect(); // 총 이펙트  
	void LineTrace();  // 라인트레이스

// UFUNCTION
	UFUNCTION(BlueprintCallable, Category = Health)
		void OnHitEvent();  // 피격 이벤트
	UFUNCTION(BlueprintCallable, Category = Health)
		void OnGameOver();  // GameOver 이벤트
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
