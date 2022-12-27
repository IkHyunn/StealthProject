// Fill out your copyright notice in the Description page of Project Settings.


#include "Clear.h"
#include <Components/BoxComponent.h>
#include "IH_Player.h"
#include "../StealthProjectGameModeBase.h"
#include "IH_ClearUI.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AClear::AClear()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Clear"));
	compBox->SetBoxExtent(FVector(250, 20, 150));

	compBox->OnComponentBeginOverlap.AddDynamic(this, &AClear::OnOverlap);
}

// Called when the game starts or when spawned
void AClear::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClear::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClear::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();

	character = Cast<AIH_Player>(OtherActor);  // BoxComponent�� Overlap�� OtherActor�� Player�̰�,

	if (currMode->currScore == 1)  // ���� ������ ���� ���� �̻��� �Ǹ�
	{
		if (character != nullptr)  // ĳ���Ͱ� null�� �ƴ� ��
		{
			UE_LOG(LogTemp, Warning, TEXT("Clear!"));
			ShowGameClearUI();  // GameClear UI�� ���̰�
			UGameplayStatics::SetGamePaused(GetWorld(), true);  // ������ Paused �ǰ�
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);  // ���콺 Ŀ���� ���δ�.
		}
	}
	else  // �ƴϸ�
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Yet!"));  // Ŭ���� ���� �ʴ´�.
	}
}

void AClear::ShowGameClearUI()
{
	gameClearUI = CreateWidget<UIH_ClearUI>(GetWorld(), gameClearWidget);

	gameClearUI->AddToViewport();
}