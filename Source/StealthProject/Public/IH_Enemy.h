// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IH_Enemy.generated.h"

UCLASS()
class STEALTHPROJECT_API AIH_Enemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AIH_Enemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=FSMComponent)
	class UEnemyFSM* fsm;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* compBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ATPSPlayer* character;  // Overlap 된 OtherActor가 캐릭터일 떄 저장해둘 변수 선언

	UPROPERTY(EditAnywhere)
	class UBoxComponent* compHandBox;

	UPROPERTY(EditAnywhere)
	class USceneComponent* compEye;

	UPROPERTY(EditAnywhere)
	class USceneComponent* compSpine;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* assasinateCam;

	UPROPERTY(EditAnywhere)
	class USphereComponent* compSphere;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* compWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEnemyHP> enemyHPFactory;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* compAlarmWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTextAlarmUI> alarmFactory;

	UPROPERTY(EditAnywhere)
	class UEnemyHP* enemyHPUI;

 	UFUNCTION()
 	void OnBackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBackEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
