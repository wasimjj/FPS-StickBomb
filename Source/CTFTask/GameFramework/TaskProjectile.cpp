// Copyright Epic Games, Inc. All Rights Reserved.

#include "TaskProjectile.h"

#include "TaskCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ACTFTaskProjectile::ACTFTaskProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	//Registering hit event on the server only .
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;
	//if (GetLocalRole() == ROLE_Authority)
	//{
	CollisionComp->OnComponentHit.AddDynamic(this, &ACTFTaskProjectile::OnHit);		// set up a notification for when this component hits something blocking
	//}
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 6000.f;
	ProjectileMovement->MaxSpeed = 6000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	DamageType = UDamageType::StaticClass();
	DamageValue = 20.0f;
}

void ACTFTaskProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if(HasAuthority())
	{
		if ((OtherActor != nullptr) && (OtherActor != this) )
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
			if(APawn* Pawn = GetInstigator())
			{
				//UGameplayStatics::ApplyPointDamage(OtherActor, DamageValue, NormalImpulse, Hit, Pawn->Controller, this, DamageType);

			}
			if(ACTFTaskCharacter*  Character = dynamic_cast<ACTFTaskCharacter*>(OtherActor))
			{
				Character->SetBombVisibility(true);
			}
			GLog->Log(FString::Printf(TEXT("OnHit ...")));
			Destroy();
		}
	}
	
}

