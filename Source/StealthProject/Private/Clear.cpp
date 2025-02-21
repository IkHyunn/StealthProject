// Fill out your copyright notice in the Description page of Project Settings.


#include "Clear.h"
#include <Components/BoxComponent.h>
#include "../StealthProjectGameModeBase.h"
#include "IH_ClearUI.h"
#include <Kismet/GameplayStatics.h>
#include "TPSPlayer.h"

// Sets default values
AClear::AClear()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Clear"));
	compBox->SetBoxExtent(FVector(250, 20, 150));

	ConstructorHelpers::FClassFinder<UIH_ClearUI>tempclear(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_ClearUI.WG_ClearUI_C'"));
	if (tempclear.Succeeded())
	{
		gameClearWidget = tempclear.Class;
	}
}

// Called when the game starts or when spawned
void AClear::BeginPlay()
{
	Super::BeginPlay();
	
	compBox->OnComponentBeginOverlap.AddDynamic(this, &AClear::OnOverlap);

	gameClearUI = CreateWidget<UIH_ClearUI>(GetWorld(), gameClearWidget);
}

// Called every frame
void AClear::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClear::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AStealthProjectGameModeBase* currMode = GetWorld()->GetAuthGameMode<AStealthProjectGameModeBase>();

	character = Cast<ATPSPlayer>(OtherActor);  // BoxComponent에 Overlap된 OtherActor가 Player이고,

	if (currMode->currScore == 5)  // 현재 점수가 일정 점수 이상이 되면
	{
		if (character != nullptr)  // 캐릭터가 null이 아닐 때
		{
			if (gameClearUI->IsInViewport() == false)
			{
// 				UGameplayStatics::SetGamePaused(GetWorld(), true);  // 게임이 Paused 되고
// 				GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);  // 마우스 커서가 보인다.
				gameClearUI->AddToViewport();
			}
		}
	}
}