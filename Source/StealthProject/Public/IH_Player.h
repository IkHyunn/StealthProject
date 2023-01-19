// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IH_Player.generated.h"

UCLASS()
class STEALTHPROJECT_API AIH_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AIH_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* tpsCamComp;

	void Turn(float value);		// �¿� ȸ�� �Է� ó��
	void LookUp(float vluae);	// ���� ȸ�� �Է� ó��

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
		float walkSpeed = 600;	// �̵� �ӵ�

	FVector direction;		// �̵� ����

	void InputHorizontal(float value);	// �¿� �̵� �Է� ó��
	void InputVertical(float value);	// ���� �̵� �Է� ó��

	void InputJump();	// ���� �Է� ó��
//	void InputAttack();

	void Move();

/*	void InputAssasinate();*/

	UPROPERTY(EditAnywhere)
	bool isBack = false;
	
	float currentTime = 0;

	UPROPERTY(EditAnywhere)
	float attackDelayTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = Helath)
	int32 HP;  // ���� HP

	UPROPERTY(EditDefaultsOnly, Category = Helath)
	int32 initialHP = 5;  // ���� HP

// 	UFUNCTION(BlueprintCallable, Category = Health)
// 	void OnHitEvent();

// 	UFUNCTION(BlueprintCallable, Category = Health)
// 	void OnGameOver();
};
