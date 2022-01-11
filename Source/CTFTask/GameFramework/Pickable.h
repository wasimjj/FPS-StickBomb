// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskHUD.h"
#include "GameFramework/Actor.h"
#include "Pickable.generated.h"

UCLASS()
class CTFTASK_API APickable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickable();
	UPROPERTY(BlueprintAssignable)
	FOnItemPicked OnItemPicked;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Flag")
	UStaticMeshComponent* PickableMesh;
	UFUNCTION(BlueprintCallable, Category="Overlap")
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
										 const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



};
