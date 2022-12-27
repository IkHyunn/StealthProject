// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Clear.generated.h"

UCLASS()
class STEALTHPROJECT_API AClear : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClear();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* compBox;

	UPROPERTY(EditAnywhere)
	class AIH_Player* character;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UIH_ClearUI> gameClearWidget;  // UIH_ClearUI Ŭ������ gameClearWidget ������ ��´�.

	UPROPERTY()
	class UIH_ClearUI* gameClearUI;  // UIH_Clear Ŭ������ ���� ������ ���� gameClearUI�� �����Ѵ�.

	void ShowGameClearUI();
};
