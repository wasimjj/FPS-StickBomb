// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"

#include "Kismet/GameplayStatics.h"

AAmmo::AAmmo()
{
	
}
void AAmmo::BeginPlay()
{
	Super::BeginPlay();
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (PlayerController->GetLocalRole() == ROLE_Authority)
		{
			PickableMesh->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnComponentBeginOverlap);
		}
	}
}

void AAmmo::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentBeginOverlap( OverlappedComponent,  OtherActor,
									 OtherComp,  OtherBodyIndex,  bFromSweep,SweepResult);
	GLog->Log("Ammo printing.......");
}



