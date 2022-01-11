// Fill out your copyright notice in the Description page of Project Settings.


#include "BombComponent.h"

// Sets default values for this component's properties
UBombComponent::UBombComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBombComponent::BeginPlay()
{
	Super::BeginPlay();
	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	//FlagMesh->SetupAttachment(RootComponent);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagMesh->SetOnlyOwnerSee(false);
	FlagMesh->SetOwnerNoSee(true);
	FlagMesh->CastShadow = false;
	FlagMesh->SetVisibility(false);
	//BlinkMaterial = UMaterialInstanceDynamic::Create(FlagMesh->GetMaterial(0),nullptr);
	//FlagMesh->SetMaterial(0,BlinkMaterial);
	// ...
	
}


// Called every frame
void UBombComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

