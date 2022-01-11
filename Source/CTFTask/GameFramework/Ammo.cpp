// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"

#include "Kismet/GameplayStatics.h"

AAmmo::AAmmo()
{
	
}
void AAmmo::BeginPlay()
{
	Super::BeginPlay();
	if (const ACTFTaskCharacter* PlayerController = Cast<ACTFTaskCharacter>( UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		//if (PlayerController->IsLocallyControlled())
		{
			PickableMesh->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnComponentBeginOverlap);
			PickableMesh->OnComponentEndOverlap.AddDynamic(this, &AAmmo::OnComponentEndOverlap);
		}
	}
}

void AAmmo::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentBeginOverlap( OverlappedComponent,  OtherActor,
									 OtherComp,  OtherBodyIndex,  bFromSweep,SweepResult);
	if(OtherActor->GetLocalRole() == ROLE_Authority)
		GLog->Log("OnComponentBeginOverlap");
	
}

void AAmmo::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnComponentEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if(OtherActor->GetLocalRole() == ROLE_Authority)
	GLog->Log("OnComponentEndOverlap.......");
	
}

void AAmmo::DestroyOnServer_Implementation()
{
	Destroy();
}




