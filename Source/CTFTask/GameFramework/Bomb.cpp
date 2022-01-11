// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ABomb::ABomb()
{
}

 void ABomb::BeginPlay()
 {
 	Super::BeginPlay();
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (PlayerController->GetLocalRole() == ROLE_Authority)
		{
 			PickableMesh->OnComponentBeginOverlap.AddDynamic(this, &ABomb::OnComponentBeginOverlap);
 		}
 	}
 }

void ABomb::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentBeginOverlap( OverlappedComponent,  OtherActor,
									 OtherComp,  OtherBodyIndex,  bFromSweep,SweepResult);
	GLog->Log("Bomb printing.......");
}

