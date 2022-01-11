// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickable.h"

#include "EngineUtils.h"
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
	if (const ACharacter* Character =  UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		//if (Character->IsLocallyControlled())
		{
			PickableMesh->OnComponentBeginOverlap.AddDynamic(this, &APickable::OnComponentBeginOverlap);
			PickableMesh->OnComponentEndOverlap.AddDynamic(this, &APickable::OnComponentEndOverlap);
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
			if (CTFTaskCharacter->IsLocallyControlled())
			{
				OnItemPickable.Broadcast(this);
			}
			
		}
	}
}

void APickable::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr)
	{
		if (ACTFTaskCharacter* CTFTaskCharacter = Cast<ACTFTaskCharacter>(OtherActor))
		{
			if (CTFTaskCharacter->IsLocallyControlled())
			{
				OnItemNotPickable.Broadcast(this);
			}
			
		}
	}
}

