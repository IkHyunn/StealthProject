
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

	// 가상함수 선언부 ----------------------------------------------------------------------------------------

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;  //폰을 상속받아 자동으로 오버라이드 한다- 
	


public:
	// 컴포넌트 멤버변수 선언부 ------------------------------------------------------------------------------
	
	// 1- 스프링암 
	UPROPERTY(VisibleAnywhere, Category = Camera)   // 클래스 전방선언, (#include 하지 않고, 컴파일 빠르게 하기 위함) & 포인터 변수 앞에만 class 가능,
	class USpringArmComponent* springArmComp;     // 포인터변수, 메모리관리를 언리얼로부터 동적으로 받을 수 있도록 uproperty 선언함
	                                             //class 타입 *포인터변수
	// 1- 카메라 
	UPROPERTY(VisibleAnywhere,  BlueprintReadOnly, Category = Camera)   // 6- 스나이퍼 줌인을 위한 블프READONLY
	class UCameraComponent* tpsCamComp;      //

// 	// 2- 총 메시
// 	UPROPERTY(VisibleAnywhere, Category = GunMesh)  //
// 	class USkeletalMeshComponent* gunMeshComp;

	// 3- 총알공장
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)    // 
	TSubclassOf<class ABullet>bulletFactory;    // 템플릿에 등록된 블루프린트 클래스를 에디터로부터 할당받는다  ??

	// 4- 스나이퍼 총 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)  //
	class USkeletalMeshComponent* pistolComp;

	// 5- 현재 유탄총을 사용하고 있는지 
	bool bNoEquipped = true;   // bool 변수
	void ChangeToNoEquipped();    // 그레나데건으로
	void ChangeToPistol();     // 스나이퍼건으로

	// 6- 스나이펴 조준
	void SniperAim();     //
	bool bSniperAim = false;     // 6-1 스나이퍼 조준( press 인지 release 인지) 저장

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)    //  6-2 스나이퍼 UI 위젯을 담을 변수
	TSubclassOf<class UUserWidget> sniperUIFactory;     

	UPROPERTY()
	class UUserWidget* _sniperUI;    // 스나이퍼 ui 위젯 인스턴스

	UPROPERTY(EditAnywhere, Category = BulletEffect)   // 6-3 총알 파편효과 공장
	class UParticleSystem* bulletEffectFactory;  

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)   //  6-4 일반 조준 크로스헤어UI 속성 추가
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUserWidget* _crosshairUI;     // 크로스헤어 인스턴스
		
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // 10-걷기 속도
	float walkSpeed = 375;                 
	
	UPROPERTY(EditAnywhere, Category = PlayerSetting)     // 10- 달리기 속도
	float runSpeed = 600;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float crouchSpeed = 200;

	UPROPERTY()
	class UPlayerAnim* anim;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)     //카메라세이크 블루프린트를 저장할 변수
	TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditAnywhere)
	int32 currentBullet = 0;

	UPROPERTY(EditAnywhere)
	bool bgetGun = false;

	UPROPERTY(EditDefaultsOnly, Category=Sound)   //총알 발사 사운드
	class USoundBase* bulletSound;

	UPROPERTY(EditAnywhere)
	bool isBack = false;  // 뒤에서 공격할 때 사용하는 bool 변수

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

	
	// 함수 선언부 ---------------------------------------------------------------------------

	void Turn(float value);    // 1-좌우회전
	void LookUp(float value);    //  1- 상하회전
	FVector direction = FVector::ZeroVector;   //  1- 방향 direction = 제로 초기화 (FVector::ZeroVector) , 멤버변수로 지정,벡터변수  ??
	void InputHorizontal(float value);  //  1- 좌우이동
	void InputVertical(float value);     //  1- 상하이동
	void InputJump();   // 1- 점프
	void Move();      //. 1- 무브
	void InputFire();   //  3- 발사
	void InputRun();   // 10- 달리기
	void InputCrouch(); // 숙이기

//	void InputAttack(); // 공격
	void InputAssasinate();  // 암살

	UFUNCTION(BlueprintCallable, Category = Health)
		void OnHitEvent();  // 피격 이벤트

	UFUNCTION(BlueprintCallable, Category = Health)
		void OnGameOver();  // GameOver 이벤트
		 
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
