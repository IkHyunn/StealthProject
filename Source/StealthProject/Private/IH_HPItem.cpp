// Fill out your copyright notice in the Description page of Project Settings.


#include "IH_HPItem.h"
#include <Components/BoxComponent.h>
#include "TPSPlayer.h"
#include <Components/SphereComponent.h>
#include <Components/WidgetComponent.h>
#include <Blueprint/UserWidget.h>
#include "PlayerHP.h"

// Sets default values
AIH_HPItem::AIH_HPItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(compBox);
	compBox->SetBoxExtent(FVector(50));

	compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	compMesh->SetupAttachment(compBox);
	compMesh->SetRelativeScale3D(FVector(2));
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/Wise/Resources/StorageHouse/Meshes/Props/SM_FirstAidKit.SM_FirstAidKit'"));
	if (tempMesh.Succeeded())
	{
		compMesh->SetStaticMesh(tempMesh.Object);
	}

	compSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	compSphere->SetupAttachment(RootComponent);
	compSphere ->SetSphereRadius(450);

	compWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pointer"));
	compWidget->SetupAttachment(RootComponent);
	compWidget->SetRelativeLocation(FVector(15, 0, 40));

	ConstructorHelpers::FClassFinder<UUserWidget> tempWidget(TEXT("WidgetBlueprint'/Game/Wise/Widget/WG_Pointer.WG_Pointer_C'"));
	if (tempWidget.Succeeded())
	{
		compWidget->SetWidgetClass(tempWidget.Class);
		compWidget->SetWidgetSpace(EWidgetSpace::Screen);
		compWidget->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void AIH_HPItem::BeginPlay()
{
	Super::BeginPlay();

	compBox->OnComponentBeginOverlap.AddDynamic(this, &AIH_HPItem::OnOverlap);
	compSphere->OnComponentEndOverlap.AddDynamic(this, &AIH_HPItem::OnEndOverlap);
	compSphere->OnComponentBeginOverlap.AddDynamic(this, &AIH_HPItem::OnSphereOverlap);
	compSphere->OnComponentEndOverlap.AddDynamic(this, &AIH_HPItem::OnSphereEndOverlap);

}

// Called every frame
void AIH_HPItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIH_HPItem::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		character->pickHPItem=this;
// 		character->HP += 3;
// 
// 		if (character->HP >= 5)
// 		{
// 			character->HP=character->maxHP;
// 			character->playerHPUI->UpdatePlayerHP(character->maxHP, character->maxHP);
// 		}
// 		else
// 		{
// 			character->playerHPUI->UpdatePlayerHP(character->HP, character->maxHP);
// 		}
// 
// 		Destroy();
	}
}

void AIH_HPItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		character->pickHPItem = nullptr;
	}
}

void AIH_HPItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(true);
	}
}

void AIH_HPItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	character = Cast<ATPSPlayer>(OtherActor);

	if (character != nullptr)
	{
		compWidget->SetVisibility(false);
	}
}