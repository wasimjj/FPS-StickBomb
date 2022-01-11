// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickable.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
APickable::APickable()
{
	PickableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickableMesh"));
	PickableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickableMesh->SetCollisionProfileName("Custom");
}

// Called when the game starts or when spawned
void APickable::BeginPlay()
{
	Super::BeginPlay();
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (PlayerController->GetLocalRole() == ROLE_Authority)
		{
			PickableMesh->OnComponentBeginOverlap.AddDynamic(this, &APickable::OnComponentBeginOverlap);
		}
	}
}

void APickable::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		if (ACTFTaskCharacter* CTFTaskCharacter = Cast<ACTFTaskCharacter>(OtherActor))
		{
//			if (CTFTaskCharacter->TaskPlayerState->CurrentHealth > 0)
			{
				GLog->Log("printing.......");
				SetActorHiddenInGame(true);
				OnItemPicked.Broadcast();
			}
		}
	}
}
